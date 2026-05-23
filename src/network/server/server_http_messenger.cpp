#include "server_http_messenger.hpp"
#include <QFile>
#include "json_utils.hpp"

Q_LOGGING_CATEGORY(server_http_messenger, "server.messenger.http")

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
    m_httpServer.router()->addConverter<RestPath>(u".+");

    routeConnection();
    routeFileSystem();

    qCDebug(server_http_messenger) << "ServerHttpMessenger - created";
}

ServerHttpMessenger::~ServerHttpMessenger()
{
    qCDebug(server_http_messenger) << "ServerHttpMessenger - destroyed";
}

bool ServerHttpMessenger::startDefault()
{
    qCDebug(server_http_messenger) << "Starting a default server";

    if (m_defaultEnabled) {
        qCInfo(server_http_messenger) << "HTTP message server is already running";
        return true;
    }
    return startServer(m_tcpServer, m_tcpPort);
}

void ServerHttpMessenger::stopDefault()
{
    qCDebug(server_http_messenger) << "Stopping a default server";

    if (!m_defaultEnabled) {
        qCInfo(server_http_messenger) << "HTTP message server has already stopped";
        return;
    }

    bool secure = false;
    stopServer(secure);
}

bool ServerHttpMessenger::startSecure()
{
    qCDebug(server_http_messenger) << "Starting a secure server";

    if (m_secureEnabled) {
        qCInfo(server_http_messenger) << "HTTPS message server is already running";
        return true;
    }
    return startServer(m_sslServer, m_sslPort);
}

void ServerHttpMessenger::stopSecure()
{
    qCDebug(server_http_messenger) << "Stopping a secure server";

    if (!m_secureEnabled) {
        qCInfo(server_http_messenger) << "HTTPS message server has already stopped";
        return;
    }

    bool secure = true;
    stopServer(secure);
}

template<typename Server>
constexpr QStringView ServerHttpMessenger::getProtocolName()
{
    qCDebug(server_http_messenger) << "Getting the protocol name";

    QStringView protocolName;
    if constexpr (std::is_same_v<Server, QTcpServer>) {
        protocolName = u"HTTP";
    } else {
        protocolName = u"HTTPS";
    }
    return protocolName;
}

void ServerHttpMessenger::stopServer(bool stopSecretServer)
{
    qCDebug(server_http_messenger) << "Stopping server";

    bool isSecureConfig;
    for (auto serverList = m_httpServer.servers(); auto &server : serverList) {
        if (qobject_cast<QSslServer *>(server) == nullptr) {
            isSecureConfig = false;
        } else {
            isSecureConfig = true;
        }

        if (!isSecureConfig && !stopSecretServer) {
            m_tcpServer.reset(server);
            stopServer(m_tcpServer, m_tcpPort);
            m_defaultEnabled = false;
        }
        if (isSecureConfig && stopSecretServer) {
            m_sslServer.reset(qobject_cast<QSslServer *>(server));
            stopServer(m_sslServer, m_sslPort);
            m_secureEnabled = false;
        }
    }
}

template<typename Server>
bool ServerHttpMessenger::startServer(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    qCDebug(server_http_messenger) << "Starting the" << protocolName << "server";

    if (!server->listen(QHostAddress::AnyIPv4, port) || !m_httpServer.bind(server.get())) {
        qCritical() << protocolName << "server cannot start. (Port" << port << "may be busy)";
        return false;
    }
    server.release();

    qCInfo(server_http_messenger) << protocolName << "messaging server is running on port:" << port;

    if constexpr (std::is_same_v<Server, QTcpServer>) {
        m_defaultEnabled = true;
    } else {
        m_secureEnabled = true;
    }

    return true;
}

template<typename Server>
void ServerHttpMessenger::stopServer(std::unique_ptr<Server> &server, quint16 port)
{
    auto protocolName = getProtocolName<Server>();

    qCDebug(server_http_messenger) << "Stopping the" << protocolName << "server";

    server->close();
    server.get()->setParent(nullptr);

    qCInfo(server_http_messenger) << protocolName << "message server was stopped on port:" << port;
}

void ServerHttpMessenger::routeConnection()
{
    qCDebug(server_http_messenger) << "Installing a server health check handler";

    m_httpServer.route("/",
                       QHttpServerRequest::Method::Head,
                       [this](const QHttpServerRequest &request) {
                           qCInfo(server_http_messenger)
                               << "HEAD request received:" << request.url().url();

                           QHttpServerResponse response(QHttpServerResponse::StatusCode::Ok);

                           QHttpHeaders responseHeader;
                           responseHeader.append(QHttpHeaders::WellKnownHeader::Server, m_hostKey);

                           response.setHeaders(responseHeader);

                           return response;
                       });
}

void ServerHttpMessenger::routeFileSystem()
{
    qCDebug(server_http_messenger) << "Installing a directory retrieval handler";

    m_httpServer.route("/?<arg>",
                       QHttpServerRequest::Method::Get,
                       [](const RestPath &path, const QHttpServerRequest &request) {
                           qCInfo(server_http_messenger)
                               << "GET request received:" << request.url().url();

                           auto &headers = request.headers();
                           if (headers.contains("Accept")) {
                               qCDebug(server_http_messenger)
                                   << "Accept:" << headers.value("Accept");

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
    qCDebug(server_http_messenger) << "Installing a missing message handler";

    m_httpServer.setMissingHandler(this, [](const QHttpServerRequest &request) {
        qCWarning(server_http_messenger) << "Unknown request:" << request.query().toString();

        QHttpServerResponse response(QHttpServerResponse::StatusCode::BadRequest);
        return response;
    });
}
