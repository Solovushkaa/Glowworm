#include "httpclient.h"
#include "jsonhelperfunctions.h"

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_downloadManager()
{
    connect(&m_timer, &QTimer::timeout, this, &HttpClient::startConnectionVerification);
}

void HttpClient::checkConnectionToServer()
{
    qDebug() << "Check connection to server:";

    QNetworkRequest request(m_url);
    QNetworkReply *reply = m_networkManager->head(request);

    connect(reply, &QNetworkReply::finished, this, &HttpClient::onCheckConnectionReceived);
}

void HttpClient::getDirectoryList(const QString &path)
{
    /*
     Загрузка старого окна, а после отсылка нового реквеста
     и уже потом получение и назначение нового списка. В таком
     случае при изменении будет заметно обновление, а при его
     отсутствии не будет никакого перехода

     if(fileGraph.find(path) != fileGraph.end()){
        send fileGraph[path] to FrontEnd
     }
    */
    QNetworkRequest request(m_url.toString() + path);
    request.setRawHeader("Accept", "application/json");
    qDebug() << "Request active";

    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, &HttpClient::onDirectoryReceived);
}

void HttpClient::getFile(const QString &path, const QString &savePath, const QString &saveName)
{
    qDebug() << "-------------------------------------------------";
    qDebug() << "Начало отправки запросов на получение файла";

    QVariantHash *fileInfo;
    for (auto &file : m_currentDirectory) {
        if (file["path"] == path) {
            qDebug() << "File finded and already exist for Downloading!";
            fileInfo = &file;
        }
    }
    QVariantHash &refFileInfo = *fileInfo;

    QString downloadID(
        QCryptographicHash::hash((path + m_currentHostKey).toUtf8(), QCryptographicHash::Md5)
            .toHex());

    if (downloadID.size() > 8) {
        downloadID = downloadID.left(8);
    }

    auto downloadInfoList = m_downloadManager.get_dowloadInfoList();

    // Можно запихать создание объекта и прочее в DownloadInfo
    if (!downloadInfoList.contains(downloadID)) {
        DownloadInfo downloadInfo(downloadID,
                                  m_url,
                                  m_currentHostKey,
                                  refFileInfo["name"].toString(),
                                  refFileInfo["path"].toString(),
                                  saveName,
                                  savePath,
                                  refFileInfo["size"].toLongLong(),
                                  0,
                                  refFileInfo["created"].toString(),
                                  refFileInfo["modified"].toString(),
                                  refFileInfo["accessed"].toString(),
                                  State::Active);

        m_downloadManager.addDownloadToUnfinished(downloadInfo);

        downloadInfoList.emplace(downloadID, std::move(downloadInfo));

        emit newDownload(saveName, downloadID, downloadInfoList[downloadID].m_fileSize);

        startDownload(downloadID);
    } else {
        qDebug().nospace() << downloadID << ": уже добавлен в загрузки!";
    }
}

void HttpClient::startDownload(const QString &downloadID)
{
    auto downloadInfoList = m_downloadManager.get_dowloadInfoList();

    qDebug() << "downloadID:" << downloadInfoList[downloadID].m_downloadID;
    qDebug() << "Получение: bytes=" << downloadInfoList[downloadID].m_fileLastReceivedByte << "-"
             << downloadInfoList[downloadID].m_fileSize;

    downloadInfoList[downloadID].setDownloadStatus(State::Active);
    m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);
    m_downloadManager.addDownloadToUnfinished(downloadInfoList[downloadID]);

    /*
     * Добавить здесь поиск нужного хоста и в случае его отсутствия дать уведомление с подсказкой с какого устройства была загрузка
     */
    QNetworkRequest request(m_url.toString() + downloadInfoList[downloadID].m_filePath);
    request.setRawHeader("Accept", "application/octet-stream");
    request.setRawHeader("Range",
                         "bytes="
                             + QByteArray::number(
                                 downloadInfoList[downloadID].m_fileLastReceivedByte)
                             + "-" + QByteArray::number(downloadInfoList[downloadID].m_fileSize));
    QNetworkReply *reply = m_networkManager->get(request);

    reply->setProperty("downloadID", downloadID);

    connect(reply, &QNetworkReply::downloadProgress, this, &HttpClient::onDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &HttpClient::onRangeReceived);

    // stopDownload(downloadID);
}

void HttpClient::stopDownload(const QString &downloadID)
{
    auto downloadInfoList = m_downloadManager.get_dowloadInfoList();

    // QNetworkRequest request(downloadInfoList[downloadID].m_URL+"/stop/...");

    // QNetworkReply *reply = m_networkManager->post(request, "stop/...");

    downloadInfoList[downloadID].setDownloadStatus(State::Pause);
}

/*
 * Присоединение лямбд к соответствующим сигналам
 */
void HttpClient::requestAnswersHandler()
{
    
}

void HttpClient::onCheckConnectionReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->url() == m_url) {
        if (!reply) {
            qDebug() << "Empty pointer to reply";
            reply->close();
            return;
        }

        if (reply->error() == QNetworkReply::NoError) {
            int currentStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                                        .toInt();
            m_currentHostKey = reply->header(QNetworkRequest::ServerHeader).toString();

            emit changeStatusCode(currentStatusCode);

            qDebug() << "Current Status Code: " << currentStatusCode;
        }
    }
    reply->close();
}

void HttpClient::onDirectoryReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(!reply){
        qDebug() << "Empty pointer to reply";
        return;
    }

    if(reply->error() == QNetworkReply::NoError){
        qDebug() << "Directory received!";

        QByteArray data = reply->readAll();

        m_currentDirectory = fromJsonToHash(QJsonDocument::fromJson(data));

        emit requestSuccessfulFinished();
    } else {
        emit requestError("Request Error...");
    }
}

void HttpClient::onRangeReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(!reply){
        qDebug() << "Empty pointer to reply";
        qDebug() << "-------------------------------------------------";
        return;
    }

    if(reply->error() == QNetworkReply::NoError){
        qDebug() << "Get-Range Answer";
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        QByteArray data = reply->readAll();

        QString downloadID = reply->property("downloadID").toString();

        auto downloadInfoList = m_downloadManager.get_dowloadInfoList();

        QString path = downloadInfoList[downloadID].m_filePath;
        QString savePath = downloadInfoList[downloadID].m_fileSavePath;
        QString saveName = downloadInfoList[downloadID].m_fileSaveName;

        qDebug() << "Место сохранения: " << savePath+saveName;

        QFile file(savePath + saveName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qWarning() << "Не удалось открыть файл для дозаписи:" << file.errorString();
            qDebug() << "-------------------------------------------------";
            return;
        } else {
            file.write(data);
            file.close();
        }

        QByteArray contenRange = reply->rawHeader("Content-range");

        if(contenRange.isEmpty()){
            qWarning() << "Нет заголовка Content-Range";
            qDebug() << "-------------------------------------------------";
            return;
        }

        QRegularExpression rx("^bytes (\\d+)-(\\d+)/(\\d+)$");
        QRegularExpressionMatch match = rx.match(QString::fromUtf8(contenRange));

        qint64 start, end, length;
        if(match.hasMatch()){
            start = match.captured(1).toLongLong();
            end = match.captured(2).toLongLong();
            length = match.captured(3).toLongLong();
        } else{
            qWarning() << "Content-Range не соответствует ожидаемому формату:" << contenRange;
            qDebug() << "-------------------------------------------------";
            return;
        }

        if(start > end || end >= length){
            qWarning() << "Content-Range не соответствует ожидаемому значению:" << contenRange;
        }

        if(end+1 == length){
            m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);

            emit rangeRequestSuccessfulFinished(downloadID);
            qDebug() << "удалённый файл успешно получен";
            qDebug() << "-------------------------------------------------";
            return;
        } else {
            startDownload(downloadID);
            return;
        }

    } else {
        emit requestError("Request Error...");
        qDebug() << "-------------------------------------------------";
    }
}

void HttpClient::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString downloadID = reply->property("downloadID").toString();

    qDebug() << "Download progress: Bytes Received -" << bytesReceived << "B";

    auto downloadInfoList = m_downloadManager.get_dowloadInfoList();

    if (downloadInfoList[downloadID].getDownloadStatus() == State::Pause
        && bytesReceived != bytesTotal) {
        QByteArray data = reply->readAll();
        reply->close();
        qDebug() << "Text after reply->readAll()";

        QString path = downloadInfoList[downloadID].m_filePath;
        QString savePath = downloadInfoList[downloadID].m_fileSavePath;
        QString saveName = downloadInfoList[downloadID].m_fileSaveName;

        QFile file(savePath + saveName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qWarning() << "Не удалось открыть файл для дозаписи:" << file.errorString();
            qDebug() << "-------------------------------------------------";
            return;
        } else {
            file.write(data);
            downloadInfoList[downloadID].m_fileLastReceivedByte = file.size();
            m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);
            m_downloadManager.addDownloadToUnfinished(downloadInfoList[downloadID]);

            file.close();
        }

        emit stopProgress(downloadID, downloadInfoList[downloadID].m_fileLastReceivedByte);
    } else {
        downloadInfoList[downloadID].m_fileLastReceivedByte = bytesReceived;

        emit changeProgress(downloadID, bytesReceived);
    }
}
