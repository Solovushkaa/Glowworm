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
    qDebug() << "Create Server HTTP Messenger";
    m_httpServer.router()->addConverter<RestPath>(u".+");

    routeConnection();
    routeFileSystem();
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

template<typename Server>
bool ServerHttpMessenger::start(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    if (!server->listen(QHostAddress::AnyIPv4, port) || !m_httpServer.bind(server.get())) {
        qCritical() << protocolName << "server cannot start. (Port" << port << "may be busy)";
        return false;
    }
    server.release();

    qInfo() << protocolName << "messaging server is running on port:" << port;

    return true;
}

bool ServerHttpMessenger::start(bool useDefaultConfig, bool useSecureConfig)
{
    bool isNotError = true;
    if (useDefaultConfig) {
        isNotError &= start(m_tcpServer, m_tcpPort);
    }
    if (useSecureConfig) {
        isNotError &= start(m_sslServer, m_sslPort);
    }

    return isNotError;
}

template<typename Server>
void ServerHttpMessenger::stop(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    server->close();
    server.get()->setParent(nullptr);

    qInfo() << protocolName << "message server was stopped on port:" << port;
}

void ServerHttpMessenger::stop(bool stopDefaultConfig, [[maybe_unused]] bool stopSecureConfig)
{
    bool isSecureConfig;
    for (auto serverList = m_httpServer.servers(); auto &server : serverList) {
        if (qobject_cast<QSslServer *>(server) == nullptr) {
            isSecureConfig = false;
        } else {
            isSecureConfig = true;
        }

        if (!isSecureConfig && stopDefaultConfig) {
            m_tcpServer.reset(server);
            stop(m_tcpServer, m_tcpPort);
        }
        if (isSecureConfig && stopDefaultConfig) {
            m_sslServer.reset(qobject_cast<QSslServer *>(server));
            stop(m_sslServer, m_sslPort);
        }
    }
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
