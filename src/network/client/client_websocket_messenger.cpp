#include "client_websocket_messenger.hpp"
#include <QJsonArray>
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(client_websocket_messenger, "client.messenger.websocket")

ClientWebSocketMessenger::ClientWebSocketMessenger(ConnectionInfo *connectionInfo,
                                                   const QByteArray &expectedFingerprint,
                                                   DirectoryManager &directoryManager,
                                                   QObject *parent)
    : QObject(parent)
    , r_connectionInfo(connectionInfo)
    , m_expectedFingerprint(expectedFingerprint)
    , r_directoryManager(directoryManager)
{
    m_pingTimer.setInterval(5000); // 5 sec
    connect(&m_pingTimer, &QTimer::timeout, this, &ClientWebSocketMessenger::sendPing);

    connect(&m_socket, &QWebSocket::connected, this, &ClientWebSocketMessenger::onConnected);
    connect(&m_socket, &QWebSocket::disconnected, this, &ClientWebSocketMessenger::onDisconnected);
    connect(&m_socket,
            &QWebSocket::textMessageReceived,
            this,
            &ClientWebSocketMessenger::onTextMessageReceived);
    connect(&m_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this,
            &ClientWebSocketMessenger::onError);
    connect(&m_socket, &QWebSocket::sslErrors, this, &ClientWebSocketMessenger::onSslError);
    connect(&m_socket, &QWebSocket::pong, this, &ClientWebSocketMessenger::onPong);

    QSslConfiguration sslConfig;
    sslConfig.setProtocol(QSsl::TlsV1_3);
    m_socket.setSslConfiguration(sslConfig);

    QUrl url;
    if (r_connectionInfo->m_connectionType == ConnectionInfo::ConnectionType::Direct) {
        url = "wss://" + r_connectionInfo->m_address + ":"
              + QString::number(r_connectionInfo->m_messengerPort);
    } else if (r_connectionInfo->m_connectionType
               == ConnectionInfo::ConnectionType::Relay) { // TODO:
        url = "wss://127.0.0.1:" + QString::number(r_connectionInfo->m_messengerPort);
    } else {
        // emit errorOccurred("Bad connection type");
    }

    m_socket.open(url);

    qCDebug(client_websocket_messenger) << "ClientWebSocketMessenger - created";
}

ClientWebSocketMessenger::~ClientWebSocketMessenger()
{
    qCDebug(client_websocket_messenger) << "ClientWebSocketMessenger - destroyed";
}

void ClientWebSocketMessenger::sendPing()
{
    if (m_socket.state() == QAbstractSocket::ConnectedState) {
        m_socket.ping("Test payload for testing WebSocket ping-pong functionality");

        QTimer::singleShot(m_pongTimeout, this, [this]() {
            ++m_missedPongs;
            if (m_missedPongs >= 2) {
                qWarning() << "Connection dead, closing...";
                m_socket.close();
            }
        });
    }
}

void ClientWebSocketMessenger::getDirectory(const QString &dirPath)
{
    qDebug() << "getDirectory";
    if (m_socket.state() == QAbstractSocket::ConnectedState) {
        QJsonObject request;
        request["action"] = "list_directory";
        request["path"] = dirPath;
        m_activeDirPath = dirPath;

        QJsonDocument doc(request);
        m_socket.sendTextMessage(doc.toJson(QJsonDocument::Compact));
    } else {
        // emit errorOccurred("Not connected to server");
    }
}

void ClientWebSocketMessenger::onTextMessageReceived(const QString &message)
{
    qCInfo(client_websocket_messenger) << "Text message received";

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject directoryJsonObject = doc.object();
        if (directoryJsonObject["error"] == "") {
            QJsonArray directoryJsonArray = directoryJsonObject["directory"].toArray();
            auto newActiveDirectory = fromJsonToFileInfo(directoryJsonArray);
            r_directoryManager.updateDirectory(std::move(newActiveDirectory),
                                               directoryJsonObject["path"].toString());

            emit currentDirectoryChanged();
        }

    } else {
        qCWarning(client_websocket_messenger) << "Invalid response";
        // emit errorOccurred("Invalid JSON response");
    }
}

// void ClientWebSocketMessenger::getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath)
// {
//     QString fullUrl = "http://" + connectionInfo->m_address + ":"
//                       + QString::number(connectionInfo->m_defaultMessengerPort) + dirPath;

//     qCInfo(client_websocket_messenger) << "Directory query by address:" << fullUrl;

//     QNetworkRequest request(fullUrl);
//     request.setRawHeader("Accept", "application/json");

//     QNetworkReply *reply = m_networkManager.get(request);

//     reply->setProperty("dirPath", dirPath);

//     connect(reply, &QNetworkReply::finished, this, &ClientWebSocketMessenger::onDirectoryReceived);

//     qCInfo(client_websocket_messenger) << "Directory fetch request created";
// }

// void ClientWebSocketMessenger::connectToRelayServer(ConnectionInfo *connectionInfo)
// {
//     QString fullUrl = "http://" + connectionInfo->m_address + ":"
//                       + QString::number(connectionInfo->m_defaultMessengerPort);

//     qCInfo(client_websocket_messenger) << "Connection to relay server:" << fullUrl;

//     QNetworkRequest request(fullUrl);
//     request.setRawHeader("Name", "user");

//     QNetworkReply *reply = m_networkManager.head(request);

//     reply->setProperty("connectionName", connectionInfo->m_name);

//     connect(reply,
//             &QNetworkReply::finished,
//             this,
//             &ClientWebSocketMessenger::onRelayStatusCodeReceived);

//     qCInfo(client_websocket_messenger) << "Connection status fetch request created";
// }

// void ClientWebSocketMessenger::networkErrorHandler(const QNetworkReply::NetworkError networkError)
// {
//     if (networkError == QNetworkReply::ConnectionRefusedError) {
//         qCWarning(client_websocket_messenger) << "Connection Error: Connection refused";
//         emit statusCodeChanged(404);
//     } else if (networkError == QNetworkReply::HostNotFoundError) {
//         qCWarning(client_websocket_messenger) << "Connection Error: Host not found";
//     } else if (networkError == QNetworkReply::TimeoutError) {
//         qCWarning(client_websocket_messenger) << "Connection Error: Timeout";
//     } else if (networkError == QNetworkReply::SslHandshakeFailedError) {
//         qCWarning(client_websocket_messenger) << "Connection Error: SSL handshake failed";
//     } else {
//         qCWarning(client_websocket_messenger) << "Connection Error: Unknown error";
//     }
// }

void ClientWebSocketMessenger::onConnected()
{
    qInfo() << "Connected to server";
    sendPing();
    m_pingTimer.start();
    // emit connected();
}

void ClientWebSocketMessenger::onDisconnected()
{
    qInfo() << "Disconnected from server";
    m_pingTimer.stop();
    // emit disconnected();
}

void ClientWebSocketMessenger::onPong(quint64 elapsedTime, const QByteArray &payload)
{
    m_missedPongs = 0;
    qDebug().nospace() << "RTT: " << elapsedTime << "ms";
    if (payload != "Test payload for testing WebSocket ping-pong functionality") {
        qWarning() << "Packet received but corrupted";
    } else {
        emit statusCodeChanged(200);
    }
}

void ClientWebSocketMessenger::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    // emit errorOccurred(m_socket.errorString());
}

void ClientWebSocketMessenger::onSslError(const QList<QSslError> &errors)
{
    disconnect(&m_socket, &QWebSocket::sslErrors, this, &ClientWebSocketMessenger::onSslError);

    qDebug() << "SSL errors:";
    for (const auto &e : errors)
        qDebug() << e.errorString();

    QSslCertificate peerCert = m_socket.sslConfiguration().peerCertificate();
    qDebug() << "Is peerCert:" << !peerCert.isNull();

    QByteArray actualFingerprint = peerCert.digest(QCryptographicHash::Sha256);
    qDebug() << "Actual Fingerprint:" << (actualFingerprint == m_expectedFingerprint);

    if (actualFingerprint == m_expectedFingerprint) {
        qDebug() << "actualFingerprint == m_expectedFingerprint";
        m_socket.ignoreSslErrors();
    } else {
        qDebug() << "Connection Failed: Bad Certificate Peer";
        m_socket.close();
    }
}

// void ClientWebSocketMessenger::onConnectionStatusCodeReceived()
// {
//     qCDebug(client_websocket_messenger)
//         << "Received a signal that the connection check is complete";

//     QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

// #ifdef QT_DEBUG
//     if (!reply) {
//         qCWarning(client_websocket_messenger) << "Empty pointer to check connection reply";
//         return;
//     }
// #endif

//     auto connectionName = reply->property("connectionName").toString();
//     auto connectionInfo = r_clientConnectionManager.getConnectionInfoDict()[connectionName];
//     int connectionIndex = r_clientConnectionManager.getConnectionInfoList().indexOf(connectionInfo);
//     if (reply->error() == QNetworkReply::NoError) {
//         qCInfo(client_websocket_messenger) << "The response from the server has been received";

//         int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//         QString currentHostKey = reply->header(QNetworkRequest::ServerHeader).toString();

//         if (currentStatusCode == 200) {
//             connectionInfo->m_hostKey = currentHostKey;

//             r_clientConnectionManager
//                 .updateConnection(connectionIndex,
//                                   constants::kConnectionState.toString(),
//                                   static_cast<int>(ConnectionInfo::ConnectionState::Connected));

//             qCInfo(client_websocket_messenger).nospace()
//                 << "Good status code from " << currentHostKey << ": " << currentStatusCode;
//         } else {
//             r_clientConnectionManager
//                 .updateConnection(connectionIndex,
//                                   constants::kConnectionState.toString(),
//                                   static_cast<int>(ConnectionInfo::ConnectionState::Disconnected));

//             qCWarning(client_websocket_messenger).nospace()
//                 << "Bad status code from " << currentHostKey << ": " << currentStatusCode;
//         }

//         emit statusCodeChanged(currentStatusCode);
//     } else {
//         r_clientConnectionManager.updateConnection(connectionIndex,
//                                                    constants::kConnectionState.toString(),
//                                                    static_cast<int>(
//                                                        ConnectionInfo::ConnectionState::Error));
//         networkErrorHandler(reply->error());

//         emit requestConnectionStatusReceivedError();
//     }

//     reply->deleteLater();
// }

// void ClientWebSocketMessenger::onDirectoryReceived()
// {
//     qCDebug(client_websocket_messenger) << "Received a signal to get a directory";

//     QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

// #ifdef QT_DEBUG
//     if (!reply) {
//         qDebug() << "Empty pointer to reply";
//         return;
//     }
// #endif

//     if (reply->error() == QNetworkReply::NoError) {
//         qDebug() << "Directory received!";

//         int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//         if (currentStatusCode == 200) {
//             QByteArray data = reply->readAll();

//             QString dirPath = reply->property("dirPath").toString();
//             auto newActiveDirectory = fromJsonToFileInfo(data);
//             r_directoryManager.updateDirectory(std::move(newActiveDirectory), dirPath);

//             qCInfo(client_websocket_messenger) << "Good status code:" << currentStatusCode;
//         } else {
//             qCWarning(client_websocket_messenger) << "Bad status code:" << currentStatusCode;
//         }

//         emit currentDirectoryChanged();
//     } else {
//         networkErrorHandler(reply->error());

//         emit requestDirectoryReceivedError();
//     }

//     reply->deleteLater();
// }

// void ClientWebSocketMessenger::onRelayStatusCodeReceived()
// {
//     qCCritical(client_websocket_messenger)
//         << "Received a signal that the RELAY server connection check is complete";

//     QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

//     if (reply->error() == QNetworkReply::NoError) {
//         qCInfo(client_websocket_messenger)
//             << "The response from the RELAY server has been received";
//         int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//         if (currentStatusCode == 200) {
//             qCInfo(client_websocket_messenger).nospace()
//                 << "Good status code from: " << currentStatusCode;
//         } else {
//             qCWarning(client_websocket_messenger).nospace()
//                 << "Bad status code from: " << currentStatusCode;
//         }
//     }
// }
