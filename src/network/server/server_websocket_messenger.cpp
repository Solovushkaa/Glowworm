#include "server_websocket_messenger.hpp"
#include <QFile>
#include <QSettings>
#include <QSslKey>
#include <QWebSocket>
#include "constants.hpp"
#include "json_utils.hpp"

Q_LOGGING_CATEGORY(server_websocket_messenger, "server.messenger.websocket")

ServerWebSocketMessenger::ServerWebSocketMessenger(const QString &hostKey, QObject *parent)
    : QObject(parent)
    , m_hostKey(hostKey)
    , m_server("GlowwormServer", QWebSocketServer::SecureMode, this)
{
    QSettings settings;
    m_port = settings.value(constants::kMessengerPortName).toInt();

    const QString serverCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    const QString serverKeyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/appdata/server/server_key.key";

    QFile serverCertificate(serverCertPath);
    QFile serverPrivateKey(serverKeyPath);
    if (!serverCertificate.open(QIODevice::ReadOnly)) {
        qFatal("Failed to load generated key/cert");
    }
    if (!serverPrivateKey.open(QIODevice::ReadOnly)) {
        qFatal("Failed to load generated key/cert");
    }

    QSslCertificate sslCert(serverCertificate.readAll(), QSsl::Pem);
    QSslKey sslKey(serverPrivateKey.readAll(), QSsl::Ec, QSsl::Pem);

    if (sslCert.isNull() || sslKey.isNull()) {
        qFatal("Failed to load generated key/cert");
    }

    QSslConfiguration sslConfig;
    sslConfig.setLocalCertificate(sslCert);
    sslConfig.setPrivateKey(sslKey);
    sslConfig.setProtocol(QSsl::TlsV1_3);

    m_server.setSslConfiguration(sslConfig);

    qCDebug(server_websocket_messenger) << "ServerWebSocketMessenger - created";
}

ServerWebSocketMessenger::~ServerWebSocketMessenger()
{
    m_server.close();
    qDeleteAll(m_clients);
    m_clients.clear();
    qCDebug(server_websocket_messenger) << "ServerWebSocketMessenger - destroyed";
}

bool ServerWebSocketMessenger::start()
{
    if (m_server.listen(QHostAddress::Any, m_port)) {
        connect(&m_server,
                &QWebSocketServer::newConnection,
                this,
                &ServerWebSocketMessenger::onNewConnection);
        qDebug() << "Server listening on port" << m_port;
    } else {
        qCritical() << "Cannot start server";
        return false;
    }

    return true;
}

void ServerWebSocketMessenger::stop()
{
    qCDebug(server_websocket_messenger) << "Stopping WebSocket server";

    m_server.close();

    for (auto *client : std::as_const(m_clients)) {
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            client->close(QWebSocketProtocol::CloseCodeGoingAway, "Server is shutting down");
            client->deleteLater();
        }
    }

    m_clients.clear();

    qCInfo(server_websocket_messenger) << "WebSocket message server has already stopped";
}

quint16 ServerWebSocketMessenger::getPort()
{
    return m_port;
}

void ServerWebSocketMessenger::onNewConnection()
{
    while (QWebSocket *socket = m_server.nextPendingConnection()) {
        connect(socket,
                &QWebSocket::textMessageReceived,
                this,
                &ServerWebSocketMessenger::onTextMessageReceived);
        connect(socket,
                &QWebSocket::disconnected,
                this,
                &ServerWebSocketMessenger::onSocketDisconnected);

        m_clients.push_back(socket);
        qDebug() << "Client connected:" << socket->peerAddress().toString();
    }
}

void ServerWebSocketMessenger::onSocketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (socket) {
        m_clients.removeOne(socket);
        socket->deleteLater();
        qDebug() << "Client disconnected";
    }
}

void ServerWebSocketMessenger::onTextMessageReceived(const QString &message)
{
    if (message.size() > 1'024) {
        qWarning() << "Message too large:" << message.size() << "bytes. Closing connection.";
        return;
    }

    QWebSocket *senderSocket = qobject_cast<QWebSocket *>(sender());
    if (!senderSocket)
        return;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        sendResponse(senderSocket, {{"error", "Invalid JSON"}, {"message", error.errorString()}});
        return;
    }

    if (!doc.isObject()) {
        sendResponse(senderSocket, {{"error", "Expected JSON object"}});
        return;
    }

    processRequest(senderSocket, doc.object());
}

void ServerWebSocketMessenger::processRequest(QWebSocket *sender, const QJsonObject &request)
{
    qDebug() << "proccessRequest()";

    QString action = request.value("action").toString();
    if (action == "list_directory") {
        QString path = request.value("path").toString();

        auto directoryInJson = createJsonFromDirectory(path);

        if (directoryInJson.isEmpty()) {
            sendResponse(sender, {{"error", "Directory does not exist"}, {"path", path}});
            return;
        }

        QJsonObject response;
        response["error"] = "";
        response["directory"] = directoryInJson;
        response["path"] = path;
        sendResponse(sender, response);
        return;
    } else {
        sendResponse(sender, {{"error", "Unknown action"}, {"action", action}});
        return;
    }

    sendResponse(sender, {{"error", "Unknown error"}});
}

void ServerWebSocketMessenger::sendResponse(QWebSocket *client, const QJsonObject &response)
{
    QJsonDocument doc(response);
    client->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}
