#include "unfinisheddownloadmanager.h"
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>

UnfinishedDownloadManager::UnfinishedDownloadManager()
{
    m_downloadInfo = readUnfinishedDownloads();
}

bool UnfinishedDownloadManager::addDownloadToUnfinished(const DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID + ".json");

    QString unfinishedDownloadsPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                    + "/AppData/Client/UnfinishedDownloads/");
    QFile file(unfinishedDownloadsPath + name);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Не получилось открыть Незавершённые загруки";
        return false;
    }

    QJsonObject jsonObject;
    jsonObject["downloadID"] = downloadInfo.m_downloadID;
    jsonObject["url"] = downloadInfo.m_URL.toString();
    jsonObject["hostKey"] = downloadInfo.m_hostKey;
    jsonObject["fileName"] = downloadInfo.m_fileName;
    jsonObject["filePath"] = downloadInfo.m_filePath;
    jsonObject["fileSaveName"] = downloadInfo.m_fileSaveName;
    jsonObject["fileSavePath"] = downloadInfo.m_fileSavePath;
    jsonObject["fileSize"] = downloadInfo.m_fileSize;
    jsonObject["fileLastReceivedByte"] = downloadInfo.m_fileLastReceivedByte;
    jsonObject["fileDateCreated"] = downloadInfo.m_fileDateCreated;
    jsonObject["fileDateLastModified"] = downloadInfo.m_fileDateLastModified;
    jsonObject["fileDateLastAccessed"] = downloadInfo.m_fileDateLastAccessed;
    jsonObject["downloadStatus"] = static_cast<int>(downloadInfo.getDownloadStatus());

    file.write(QJsonDocument(jsonObject).toJson());

    return true;
}

bool UnfinishedDownloadManager::deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID + ".json");

    QString unfinishedDownloadsPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                    + "/AppData/Client/UnfinishedDownloads/");

    if (QFile::remove(unfinishedDownloadsPath + name)) {
        qDebug() << "Файл успешно удален";
    } else {
        qDebug() << "Ошибка удаления файла";
        return false;
    }

    return true;
}

QHash<QString, DownloadInfo> UnfinishedDownloadManager::readUnfinishedDownloads()
{
    QHash<QString, DownloadInfo> downloadInfoDict;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
             + "/AppData/Client/UnfinishedDownloads/");
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    if (!fileList.empty()) {
        DownloadInfo downloadInfo;
        for (auto fileInfo : fileList) {
            QFile file(fileInfo.absoluteFilePath());

            if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                qDebug() << "UnfinishedInstallations Don't open";
            }

            QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));

            QJsonObject jsonObject = jsonDoc.object();

            downloadInfo.m_downloadID = jsonObject["downloadID"].toString();
            downloadInfo.m_hostKey = jsonObject["hostKey"].toString();
            downloadInfo.m_URL = jsonObject["url"].toString();
            downloadInfo.m_fileName = jsonObject["fileName"].toString();
            downloadInfo.m_filePath = jsonObject["filePath"].toString();
            downloadInfo.m_fileSaveName = jsonObject["fileSaveName"].toString();
            downloadInfo.m_fileSavePath = jsonObject["fileSavePath"].toString();
            downloadInfo.m_fileSize = jsonObject["fileSize"].toInteger();
            downloadInfo.m_fileLastReceivedByte = jsonObject["fileLastReceivedByte"].toInteger();
            downloadInfo.m_fileDateCreated = jsonObject["fileDateCreated"].toString();
            downloadInfo.m_fileDateLastModified = jsonObject["fileDateLastModified"].toString();
            downloadInfo.m_fileDateLastAccessed = jsonObject["fileDateLastAccessed"].toString();
            downloadInfo.setDownloadStatus(State::Pause);

            downloadInfoDict.emplace(downloadInfo.m_downloadID, std::move(downloadInfo));

            jsonObject["downloadStatus"] = static_cast<int>(State::Pause);

            file.resize(0);

            file.write(QJsonDocument(jsonObject).toJson());
        }
    }

    return downloadInfoDict;
}

QList<QVariantHash> UnfinishedDownloadManager::getVariantListUnfinishedDownloads(
    const QHash<QString, DownloadInfo> &downloadInfo)
{
    QList<QVariantHash> unfinishedDownloads;

    for (auto info : downloadInfo) {
        unfinishedDownloads.push_back(QVariantHash());
        unfinishedDownloads.back()["fileName"] = info.m_fileName;
        unfinishedDownloads.back()["downloadID"] = info.m_downloadID;
        unfinishedDownloads.back()["fileSize"] = info.m_fileSize;
        unfinishedDownloads.back()["fileLastReceivedByte"] = info.m_fileLastReceivedByte;
    }

    return unfinishedDownloads;
}
