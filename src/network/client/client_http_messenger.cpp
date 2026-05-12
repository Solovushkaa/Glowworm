#include "client_http_messenger.hpp"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "manager_utils.hpp"

ClientHttpMessenger::ClientHttpMessenger(DirectoryManager &directoryManager)
    : r_directoryManager(directoryManager)
{}

void ClientHttpMessenger::checkConnectionToServer(ConnectionInfo *connectionInfo)
{
    qInfo() << "Check connection to server:" << connectionInfo->m_url;
    QNetworkRequest request(connectionInfo->m_url);
    QNetworkReply *reply = m_networkManager.head(request);

    connect(reply,
            &QNetworkReply::finished,
            this,
            &ClientHttpMessenger::onConnectionStatusCodeReceived);
}

void ClientHttpMessenger::getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath)
{
    qCritical() << "Full path:" << connectionInfo->m_url.toString() + dirPath;
    QNetworkRequest request(connectionInfo->m_url.toString() + dirPath);
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = m_networkManager.get(request);
    reply->setProperty("dirPath", dirPath);

    qDebug() << "A new request for the directory has been generated!";
    qDebug() << "URL:" << connectionInfo->m_url;

    connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onDirectoryReceived);
}

void ClientHttpMessenger::onConnectionStatusCodeReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply) {
        qWarning() << "Empty pointer to reply";
        reply->close();
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        emit statusCodeChanged(currentStatusCode);

        if (currentStatusCode == 200) {
            // QString currentHostKey = reply->header(QNetworkRequest::ServerHeader).toString();

            // emit currentHostChanged(std::move(
            //     currentHostKey)); // Связывается со слотом в Client, который передаёт hostKey в менеджер подключений

            // qDebug().nospace() << "The status code of the response from the \"" << currentHostKey
            //                    << "\" host: " << currentStatusCode;
            qDebug() << "Status success code: " << currentStatusCode;
        } else {
            qDebug() << "Status error code: " << currentStatusCode;
        }
    }

    reply->deleteLater();
}

void ClientHttpMessenger::onDirectoryReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (!reply) {
        qDebug() << "Empty pointer to reply";
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Directory received!";

        QByteArray data = reply->readAll();

        QString dirPath = reply->property("dirPath").toString();
        auto newActiveDirectory = fromJsonToFileInfo(data);
        r_directoryManager.updateDirectory(std::move(newActiveDirectory), dirPath);

        emit currentDirectoryChanged();
        emit requestFinished();
    } else {
        emit requestError("Some error");
    }

    reply->deleteLater();
}
