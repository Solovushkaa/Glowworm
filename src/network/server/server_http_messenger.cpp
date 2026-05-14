#include "server_http_messenger.hpp"
#include <QFile>
#include "json_utils.hpp"

struct RestPath {
    QString s;
    RestPath() = default;
    RestPath(const QString &x) : s(x) {}
};

ServerHttpMessenger::ServerHttpMessenger(const QString &hostKey, quint16 tcpPort, quint16 sslPort)
    : m_hostKey(hostKey)
    , m_tcpPort(tcpPort)
    , m_sslPort(sslPort)
    , m_tcpServer(std::make_unique<QTcpServer>())
    , m_sslServer(std::make_unique<QSslServer>())
{
    qDebug() << "ServerHttpMessenger - created";
    m_httpServer.router()->addConverter<RestPath>(u".+");

    routeConnection();
    routeFileSystem();
}

bool ServerHttpMessenger::startDefaultServer()
{
    if (m_activeDefault) {
        qInfo() << "HTTP message server is already running";
        return true;
    }
    return startServer(m_tcpServer, m_tcpPort);
}

void ServerHttpMessenger::stopDefaultServer()
{
    if (!m_activeDefault) {
        qInfo() << "HTTP message server has already stopped";
        return;
    }
    stopServer(m_tcpServer, m_tcpPort);
}

bool ServerHttpMessenger::startSecureServer()
{
    if (m_activeSecure) {
        qInfo() << "HTTPS message server is already running";
        return true;
    }
    return startServer(m_sslServer, m_sslPort);
}

void ServerHttpMessenger::stopSecureServer()
{
    if (!m_activeSecure) {
        qInfo() << "HTTPS message server has already stopped";
        return;
    }
    stopServer(m_sslServer, m_sslPort);
}

template<typename Server>
constexpr QStringView ServerHttpMessenger::getProtocolName()
{
    QStringView protocolName;
    if constexpr (std::is_same_v<Server, QTcpServer>) {
        protocolName = u"HTTP";
    } else {
        protocolName = u"HTTPS";
    }
    return protocolName;
}

bool ServerHttpMessenger::startAll()
{
    bool noError = true;

    noError &= startDefaultServer();
    noError &= startSecureServer();

    return noError;
}

void ServerHttpMessenger::stopAll()
{
    bool isSecureConfig;
    for (auto serverList = m_httpServer.servers(); auto &server : serverList) {
        if (qobject_cast<QSslServer *>(server) == nullptr) {
            isSecureConfig = false;
        } else {
            isSecureConfig = true;
        }

        if (!isSecureConfig) {
            m_tcpServer.reset(server);
            stopServer(m_tcpServer, m_tcpPort);
            m_activeDefault = false;
        }
        if (isSecureConfig) {
            m_sslServer.reset(qobject_cast<QSslServer *>(server));
            stopServer(m_sslServer, m_sslPort);
            m_activeSecure = false;
        }
    }
}

template<typename Server>
bool ServerHttpMessenger::startServer(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    if (!server->listen(QHostAddress::AnyIPv4, port) || !m_httpServer.bind(server.get())) {
        qCritical() << protocolName << "server cannot start. (Port" << port << "may be busy)";
        return false;
    }
    server.release();

    qInfo() << protocolName << "messaging server is running on port:" << port;

    if constexpr (std::is_same_v<Server, QTcpServer>) {
        m_activeDefault = true;
    } else {
        m_activeSecure = true;
    }

    return true;
}

template<typename Server>
void ServerHttpMessenger::stopServer(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    server->close();
    server.get()->setParent(nullptr);

    qInfo() << protocolName << "message server was stopped on port:" << port;
}

void ServerHttpMessenger::routeConnection()
{
    m_httpServer.route("/",
                       QHttpServerRequest::Method::Head,
                       [this](const QHttpServerRequest &request) {
                           qInfo() << "HEAD request received";
                           qInfo() << request.url().toString();

                           QHttpServerResponse response(QHttpServerResponse::StatusCode::Ok);

                           QHttpHeaders responseHeader;
                           responseHeader.append(QHttpHeaders::WellKnownHeader::Server, m_hostKey);

                           response.setHeaders(responseHeader);

                           return response;
                       });
}

void ServerHttpMessenger::routeFileSystem()
{
    m_httpServer.route("/?<arg>",
                       QHttpServerRequest::Method::Get,
                       [](const RestPath &path, const QHttpServerRequest &request) {
                           qInfo() << "HEAD request received";
                           qInfo() << request.query().toString();

                           auto &headers = request.headers();
                           if (headers.contains("Accept")) {
                               qDebug() << "Accept:" << headers.value("Accept");

                               if (headers.value("Accept") == "application/json") {
                                   QHttpServerResponse response(createJsonFromDirectory("/"
                                                                                        + path.s),
                                                                QHttpServerResponse::StatusCode::Ok);

                                   return response;
                               }
                           }
                           return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
                       });
}

void ServerHttpMessenger::routeMissingHandler()
{
    m_httpServer.setMissingHandler(this, [](const QHttpServerRequest &request) {
        qWarning() << "Unknown request:";
        qWarning() << request.query().toString();

        QHttpServerResponse response(QHttpServerResponse::StatusCode::BadRequest);
        return response;
    });
}
