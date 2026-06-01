#include "client_http_messenger.hpp"
#include <QNetworkRequest>
#include "constants.hpp"
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(client_http_messenger, "client.messenger.http")

ClientHttpMessenger::ClientHttpMessenger(ClientConnectionManager &clientConnectionManager,
                                         DirectoryManager &directoryManager)
    : r_clientConnectionManager(clientConnectionManager)
    , r_directoryManager(directoryManager)
{
    qCDebug(client_http_messenger) << "ClientHttpMessenger - created";
}

ClientHttpMessenger::~ClientHttpMessenger()
{
    qCDebug(client_http_messenger) << "ClientHttpMessenger - destroyed";
}

void ClientHttpMessenger::checkConnectionToServer(ConnectionInfo *connectionInfo)
{
    QString fullUrl = "http://" + connectionInfo->m_address + ":"
                      + QString::number(connectionInfo->m_defaultMessengerPort);

    qCInfo(client_http_messenger) << "Check connection to server:" << fullUrl;

    QNetworkRequest request(fullUrl);
    QNetworkReply *reply = m_networkManager.head(request);

    reply->setProperty("connectionName", connectionInfo->m_name);

    connect(reply,
            &QNetworkReply::finished,
            this,
            &ClientHttpMessenger::onConnectionStatusCodeReceived);

    qCInfo(client_http_messenger) << "Connection status fetch request created";
}

void ClientHttpMessenger::getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath)
{
    QString fullUrl = "http://" + connectionInfo->m_address + ":"
                      + QString::number(connectionInfo->m_defaultMessengerPort) + dirPath;

    qCInfo(client_http_messenger) << "Directory query by address:" << fullUrl;

    QNetworkRequest request(fullUrl);
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = m_networkManager.get(request);

    reply->setProperty("dirPath", dirPath);

    connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onDirectoryReceived);

    qCInfo(client_http_messenger) << "Directory fetch request created";
}

void ClientHttpMessenger::connectToRelayServer(ConnectionInfo *connectionInfo)
{
    QString fullUrl = "http://" + connectionInfo->m_address + ":"
                      + QString::number(connectionInfo->m_defaultMessengerPort);

    qCInfo(client_http_messenger) << "Connection to relay server:" << fullUrl;

    QNetworkRequest request(fullUrl);
    request.setRawHeader("Name", "user");

    QNetworkReply *reply = m_networkManager.head(request);

    reply->setProperty("connectionName", connectionInfo->m_name);

    connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onRelayStatusCodeReceived);

    qCInfo(client_http_messenger) << "Connection status fetch request created";
}

void ClientHttpMessenger::networkErrorHandler(const QNetworkReply::NetworkError networkError)
{
    if (networkError == QNetworkReply::ConnectionRefusedError) {
        qCWarning(client_http_messenger) << "Connection Error: Connection refused";
    } else if (networkError == QNetworkReply::HostNotFoundError) {
        qCWarning(client_http_messenger) << "Connection Error: Host not found";
    } else if (networkError == QNetworkReply::TimeoutError) {
        qCWarning(client_http_messenger) << "Connection Error: Timeout";
    } else if (networkError == QNetworkReply::SslHandshakeFailedError) {
        qCWarning(client_http_messenger) << "Connection Error: SSL handshake failed";
    } else {
        qCWarning(client_http_messenger) << "Connection Error: Unknown error";
    }
}

void ClientHttpMessenger::onConnectionStatusCodeReceived()
{
    qCDebug(client_http_messenger) << "Received a signal that the connection check is complete";

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

#ifdef QT_DEBUG
    if (!reply) {
        qCWarning(client_http_messenger) << "Empty pointer to check connection reply";
        return;
    }
#endif

    auto connectionName = reply->property("connectionName").toString();
    auto connectionInfo = r_clientConnectionManager.getConnectionInfoDict()[connectionName];
    int connectionIndex = r_clientConnectionManager.getConnectionInfoList().indexOf(connectionInfo);
    if (reply->error() == QNetworkReply::NoError) {
        qCInfo(client_http_messenger) << "The response from the server has been received";

        int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString currentHostKey = reply->header(QNetworkRequest::ServerHeader).toString();

        if (currentStatusCode == 200) {
            connectionInfo->m_hostKey = currentHostKey;

            r_clientConnectionManager
                .updateConnection(connectionIndex,
                                  constants::kConnectionState.toString(),
                                  static_cast<int>(ConnectionInfo::ConnectionState::Connected));

            qCInfo(client_http_messenger).nospace()
                << "Good status code from " << currentHostKey << ": " << currentStatusCode;
        } else {
            r_clientConnectionManager
                .updateConnection(connectionIndex,
                                  constants::kConnectionState.toString(),
                                  static_cast<int>(ConnectionInfo::ConnectionState::Disconnected));

            qCWarning(client_http_messenger).nospace()
                << "Bad status code from " << currentHostKey << ": " << currentStatusCode;
        }

        emit statusCodeChanged(currentStatusCode);
    } else {
        r_clientConnectionManager.updateConnection(connectionIndex,
                                                   constants::kConnectionState.toString(),
                                                   static_cast<int>(
                                                       ConnectionInfo::ConnectionState::Error));
        networkErrorHandler(reply->error());

        emit requestConnectionStatusReceivedError();
    }

    reply->deleteLater();
}

void ClientHttpMessenger::onDirectoryReceived()
{
    qCDebug(client_http_messenger) << "Received a signal to get a directory";

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

#ifdef QT_DEBUG
    if (!reply) {
        qDebug() << "Empty pointer to reply";
        return;
    }
#endif

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Directory received!";

        int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (currentStatusCode == 200) {
            QByteArray data = reply->readAll();

            QString dirPath = reply->property("dirPath").toString();
            auto newActiveDirectory = fromJsonToFileInfo(data);
            r_directoryManager.updateDirectory(std::move(newActiveDirectory), dirPath);

            qCInfo(client_http_messenger) << "Good status code:" << currentStatusCode;
        } else {
            qCWarning(client_http_messenger) << "Bad status code:" << currentStatusCode;
        }

        emit currentDirectoryChanged();
    } else {
        networkErrorHandler(reply->error());

        emit requestDirectoryReceivedError();
    }

    reply->deleteLater();
}

void ClientHttpMessenger::onRelayStatusCodeReceived()
{
    qCCritical(client_http_messenger)
        << "Received a signal that the RELAY server connection check is complete";

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        qCInfo(client_http_messenger) << "The response from the RELAY server has been received";
        int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (currentStatusCode == 200) {
            qCInfo(client_http_messenger).nospace()
                << "Good status code from: " << currentStatusCode;
        } else {
            qCWarning(client_http_messenger).nospace()
                << "Bad status code from: " << currentStatusCode;
        }
    }
}
