#include "client_http_messenger.hpp"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "json_utils.hpp"

ClientHttpMessenger::ClientHttpMessenger()
    : m_networkManager(new QNetworkAccessManager(this))
{
}

void ClientHttpMessenger::checkConnectionToServer(ConnectionInfo *connectionInfo)
{
    qInfo() << "Check connection to server:";
    connectionInfo->m_url.setUrl("http://127.0.0.1:6115");
    qInfo() << connectionInfo->m_url.url();
    QNetworkRequest request(connectionInfo->m_url.url());
    QNetworkReply *reply = m_networkManager->head(request);

    connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onConnectionStatusReceived);
}

// void ClientHttpMessenger::getDirectoryList(ConnectionInfo *connectionInfo)
// {
//     /*
//      Загрузка старого окна, а после отсылка нового реквеста
//      и уже потом получение и назначение нового списка. В таком
//      случае при изменении будет заметно обновление, а при его
//      отсутствии не будет никакого перехода

//      if(fileGraph.find(path) != fileGraph.end()){
//         send fileGraph[path] to FrontEnd
//      }
//     */
//     QString absURL = url + path + "?user_id=" + userID;

//     QNetworkRequest request(absURL);
//     request.setRawHeader("Accept", "application/json");

//     QNetworkReply *reply = m_networkManager->get(request);

//     qDebug() << "A new request for the directory has been generated!";
//     qDebug() << "URL:" << absURL;

//     connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onDirectoryReceived);
// }

void ClientHttpMessenger::onConnectionStatusReceived()
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

    // reply->close(); // Есть непроверенная проблема с закрытием ответа
    reply->deleteLater();
}

// void ClientHttpMessenger::onDirectoryReceived()
// {
//     QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

//     if(!reply){
//         qDebug() << "Empty pointer to reply";
//         return;
//     }

//     if(reply->error() == QNetworkReply::NoError){
//         qDebug() << "Directory received!";

//         QByteArray data = reply->readAll();

//         QList<QVariantHash> m_currentDirectory = fromJsonToHash(data);

//         emit currentDirectoryChanged(std::move(
//             m_currentDirectory)); // Связывается со слотом в Client, который передаёт currentDirectory в сам класс

//         emit requestFinished();
//     } else {
//         emit requestError("Request Error...");
//     }

//     reply->deleteLater();
// }
