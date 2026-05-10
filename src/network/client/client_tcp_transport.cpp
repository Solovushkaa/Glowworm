// void ClientHttpMessenger::getFile(QList<QVariantHash> &currentDirectory,
//                                   const QString &currentHostKey,
//                                   DownloadManager &downloadManager,
//                                   const QString &path,
//                                   const QString &savePath,
//                                   const QString &saveName)
// {
//     qDebug() << "Sending a request to receive a file from host:" << currentHostKey;
//     qDebug() << "The received file:" << path;

//     QVariantHash *fileInfo;
//     for (auto &file : currentDirectory) {
//         if (file["path"] == path) {
//             qDebug() << "File find and already exist for Downloading!";
//             fileInfo = &file;
//         }
//     }
//     QVariantHash &refFileInfo = *fileInfo;

//     //-ДОБАВИТЬ В HELPER FUNCTIONS-//
//     QString downloadID(
//         QCryptographicHash::hash((path + currentHostKey).toUtf8(), QCryptographicHash::Md5).toHex());
//     //-----------------------------//

//     if (downloadID.size() > 8) {
//         downloadID = downloadID.left(8);
//     }

//     // // Можно запихать создание объекта и прочее в DownloadInfo
//     // QHash<QString, DownloadInfo> &downloadInfoDict = downloadManager.getDownloadInfoDict();
//     // if (!downloadInfoDict.contains(downloadID)) {
//     //     // Всё это внутри слота DownloadManager получающего newDownload

//     //     DownloadInfo downloadInfo(downloadID,
//     //                               m_url,
//     //                               m_currentHostKey,
//     //                               refFileInfo["name"].toString(),
//     //                               refFileInfo["path"].toString(),
//     //                               saveName,
//     //                               savePath,
//     //                               refFileInfo["size"].toLongLong(),
//     //                               0,
//     //                               refFileInfo["created"].toString(),
//     //                               refFileInfo["modified"].toString(),
//     //                               refFileInfo["accessed"].toString(),
//     //                               State::Active);

//     //     downloadManager.addDownloadToUnfinished(downloadInfo);

//     //     downloadInfoDict.emplace(downloadID, std::move(downloadInfo));

//     //     startDownload(downloadID);

//     //     emit newDownload(saveName, downloadID, downloadInfoDict[downloadID].m_size);
//     // } else {
//     //     qDebug().nospace() << downloadID << ": has already been added to downloads!";
//     // }
// }

// void ClientHttpMessenger::startDownload(const QString &downloadID)
// {
//     auto downloadInfoList = m_downloadManager.getDowloadInfoList();

//     qDebug() << "downloadID:" << downloadInfoList[downloadID].m_downloadID;
//     qDebug() << "Получение: bytes=" << downloadInfoList[downloadID].m_lastReceivedByte << "-"
//              << downloadInfoList[downloadID].m_size;

//     downloadInfoList[downloadID].setDownloadStatus(State::Active);
//     m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);
//     m_downloadManager.addDownloadToUnfinished(downloadInfoList[downloadID]);

//     /*
//      * Добавить здесь поиск нужного хоста и в случае его отсутствия дать уведомление с подсказкой с какого устройства была загрузка
//      */
//     QNetworkRequest request(m_url.toString() + downloadInfoList[downloadID].m_path);
//     request.setRawHeader("Accept", "application/octet-stream");
//     request.setRawHeader("Range",
//                          "bytes="
//                              + QByteArray::number(
//                                  downloadInfoList[downloadID].m_lastReceivedByte)
//                              + "-" + QByteArray::number(downloadInfoList[downloadID].m_size));
//     QNetworkReply *reply = m_networkManager->get(request);

//     reply->setProperty("downloadID", downloadID);

//     connect(reply, &QNetworkReply::downloadProgress, this, &ClientHttpMessenger::onDownloadProgress);
//     connect(reply, &QNetworkReply::finished, this, &ClientHttpMessenger::onFileReceived);

//     // stopDownload(downloadID);
// }

// void ClientHttpMessenger::stopDownload(const QString &downloadID)
// {
//     auto &downloadInfoList = m_downloadManager.getDowloadInfoList();

//     // QNetworkRequest request(downloadInfoList[downloadID].m_URL+"/stop/...");

//     // QNetworkReply *reply = m_networkManager->post(request, "stop/...");

//     downloadInfoList[downloadID].setDownloadStatus(State::Pause);
// }

// void ClientHttpMessenger::onFileReceived()
// {
//     QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

//     if(!reply){
//         qDebug() << "Empty pointer to reply";
//         qDebug() << "-------------------------------------------------";
//         return;
//     }

//     if(reply->error() == QNetworkReply::NoError){
//         qDebug() << "Get-Range Answer";
//         int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//         QByteArray data = reply->readAll();

//         QString downloadID = reply->property("downloadID").toString();

//         auto downloadInfoList = m_downloadManager.getDowloadInfoList();

//         QString path = downloadInfoList[downloadID].m_path;
//         QString savePath = downloadInfoList[downloadID].m_savePath;
//         QString saveName = downloadInfoList[downloadID].m_saveName;

//         qDebug() << "Место сохранения: " << savePath+saveName;

//         QFile file(savePath + saveName);

//         if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
//             qWarning() << "Не удалось открыть файл для дозаписи:" << file.errorString();
//             qDebug() << "-------------------------------------------------";
//             return;
//         } else {
//             file.write(data);
//             file.close();
//         }

//         QByteArray contenRange = reply->rawHeader("Content-range");

//         if(contenRange.isEmpty()){
//             qWarning() << "Нет заголовка Content-Range";
//             qDebug() << "-------------------------------------------------";
//             return;
//         }

//         QRegularExpression rx("^bytes (\\d+)-(\\d+)/(\\d+)$");
//         QRegularExpressionMatch match = rx.match(QString::fromUtf8(contenRange));

//         qint64 start, end, length;
//         if(match.hasMatch()){
//             start = match.captured(1).toLongLong();
//             end = match.captured(2).toLongLong();
//             length = match.captured(3).toLongLong();
//         } else{
//             qWarning() << "Content-Range не соответствует ожидаемому формату:" << contenRange;
//             qDebug() << "-------------------------------------------------";
//             return;
//         }

//         if(start > end || end >= length){
//             qWarning() << "Content-Range не соответствует ожидаемому значению:" << contenRange;
//         }

//         if(end+1 == length){
//             m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);

//             emit rangeRequestSuccessfulFinished(downloadID);
//             qDebug() << "удалённый файл успешно получен";
//             qDebug() << "-------------------------------------------------";
//             return;
//         } else {
//             startDownload(downloadID);
//             return;
//         }

//     } else {
//         emit requestError("Request Error...");
//         qDebug() << "-------------------------------------------------";
//     }

//     reply->deleteLater();
// }

// void ClientHttpMessenger::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
// {
//     QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

//     QString downloadID = reply->property("downloadID").toString();

//     qDebug() << "Download progress: Bytes Received -" << bytesReceived << "B";

//     auto downloadInfoList = m_downloadManager.getDowloadInfoList();

//     if (downloadInfoList[downloadID].getDownloadStatus() == State::Pause
//         && bytesReceived != bytesTotal) {
//         QByteArray data = reply->readAll();
//         reply->close();
//         qDebug() << "Text after reply->readAll()";

//         QString path = downloadInfoList[downloadID].m_path;
//         QString savePath = downloadInfoList[downloadID].m_savePath;
//         QString saveName = downloadInfoList[downloadID].m_saveName;

//         QFile file(savePath + saveName);

//         if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
//             qWarning() << "Не удалось открыть файл для дозаписи:" << file.errorString();
//             qDebug() << "-------------------------------------------------";
//             return;
//         } else {
//             file.write(data);
//             downloadInfoList[downloadID].m_lastReceivedByte = file.size();
//             m_downloadManager.deleteFromUnfinishedDownload(downloadInfoList[downloadID]);
//             m_downloadManager.addDownloadToUnfinished(downloadInfoList[downloadID]);

//             file.close();
//         }

//         emit stopProgress(downloadID, downloadInfoList[downloadID].m_lastReceivedByte);
//     } else {
//         downloadInfoList[downloadID].m_lastReceivedByte = bytesReceived;

//         emit changeProgress(downloadID, bytesReceived);
//     }
// }
