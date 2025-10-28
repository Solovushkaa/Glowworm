#include "unfinisheddownloadmanager.h"
#include <QFile>

UnfinishedDownloadManager::UnfinishedDownloadManager()
{
    if (readUnfinishedDownloads()) {
        qDebug() << "Incomplete downloads have been successfully read";
    } else {
        qWarning() << "Error reading incomplete downloads";
    }
}

bool UnfinishedDownloadManager::readUnfinishedDownloads()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Error opening the \"UnfinishedDownloads\" file";

        return false;
    }

    QJsonDocument unfinishedDownloadInfo(QJsonDocument::fromJson(file.readAll()));

    if (unfinishedDownloadInfo.isObject()) {
        m_jsonDownloadInfo = unfinishedDownloadInfo.object();
    } else {
        qWarning() << "JSON is not an object";
        return false;
    }

    DownloadInfo downloadInfo;
    for (auto &dictKey : m_jsonDownloadInfo.keys()) {
        if (!m_jsonDownloadInfo[dictKey].isObject()) {
            qWarning().nospace() << "An \"" << dictKey << "\" in json is not an object";
            return false;
        }
        const QJsonObject &object = m_jsonDownloadInfo[dictKey].toObject();

        if (!isCorrectDownloadInfoObject(object)) {
            qWarning().nospace() << "The \"" << dictKey << "\" object has an incorrect syntax";
            return false;
        }

        downloadInfo.m_downloadID = object["downloadID"].toString();
        downloadInfo.m_hostKey = object["hostKey"].toString();
        downloadInfo.m_URL = object["url"].toString();
        downloadInfo.m_fileName = object["fileName"].toString();
        downloadInfo.m_filePath = object["filePath"].toString();
        downloadInfo.m_fileSaveName = object["fileSaveName"].toString();
        downloadInfo.m_fileSavePath = object["fileSavePath"].toString();
        downloadInfo.m_fileSize = object["fileSize"].toInteger();
        downloadInfo.m_fileLastReceivedByte = object["fileLastReceivedByte"].toInteger();
        downloadInfo.m_fileDateCreated = object["fileDateCreated"].toString();
        downloadInfo.m_fileDateLastModified = object["fileDateLastModified"].toString();
        downloadInfo.m_fileDateLastAccessed = object["fileDateLastAccessed"].toString();
        downloadInfo.m_downloadStatus = State::Pause;

        m_downloadInfoDict.emplace(downloadInfo.m_downloadID, std::move(downloadInfo));

        object["downloadStatus"] = static_cast<int>(State::Pause);
    }

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    return true;
}

bool UnfinishedDownloadManager::isCorrectDownloadInfoObject(const QJsonObject &object)
{
    for (auto &name : m_checkList) {
        if (object.find(name) == object.end()) {
            qWarning().nospace() << "The \"" << name << "\" object has an incorrect name";
            return false;
        }
    }

    return true;
}

bool UnfinishedDownloadManager::addDownloadToUnfinished(DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug().nospace() << "Error opening the \"UnfinishedDownloads\" file to add \"" << name
                           << "\" download";
        return false;
    }

    const QJsonObject &object = m_jsonDownloadInfo[name].toObject();
    object["downloadID"] = downloadInfo.m_downloadID;
    object["url"] = downloadInfo.m_URL.toString();
    object["hostKey"] = downloadInfo.m_hostKey;
    object["fileName"] = downloadInfo.m_fileName;
    object["filePath"] = downloadInfo.m_filePath;
    object["fileSaveName"] = downloadInfo.m_fileSaveName;
    object["fileSavePath"] = downloadInfo.m_fileSavePath;
    object["fileSize"] = downloadInfo.m_fileSize;
    object["fileLastReceivedByte"] = downloadInfo.m_fileLastReceivedByte;
    object["fileDateCreated"] = downloadInfo.m_fileDateCreated;
    object["fileDateLastModified"] = downloadInfo.m_fileDateLastModified;
    object["fileDateLastAccessed"] = downloadInfo.m_fileDateLastAccessed;
    object["downloadStatus"] = static_cast<int>(downloadInfo.getDownloadStatus());

    m_downloadInfoDict[name] = std::move(downloadInfo);

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    qDebug() << "Added a new \"" << name << "\" download";

    return true;
}

bool UnfinishedDownloadManager::deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug().nospace() << "Error opening the \"UnfinishedDownloads\" file to delete \"" << name
                           << "\" download";
        return false;
    }

    m_downloadInfoDict.remove(name);
    m_jsonDownloadInfo.remove(name);

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    qDebug().nospace() << "The download \"" << name << "\" is finished and removed from the list";

    return true;
}

QList<QVariantHash> UnfinishedDownloadManager::getVariantListUnfinishedDownloads()
{
    QList<QVariantHash> unfinishedDownloads;

    for (auto &downloadInfo : m_downloadInfoDict) {
        unfinishedDownloads.push_back(QVariantHash());
        unfinishedDownloads.back()["fileName"] = downloadInfo.m_fileName;
        unfinishedDownloads.back()["downloadID"] = downloadInfo.m_downloadID;
        unfinishedDownloads.back()["fileSize"] = downloadInfo.m_fileSize;
        unfinishedDownloads.back()["fileLastReceivedByte"] = downloadInfo.m_fileLastReceivedByte;
    }

    return unfinishedDownloads;
}

// void UnfinishedDownloadManager::onDownloadAdded(QString &saveName,
//                                                 QString &downloadID,
//                                                 QVariantHash &refFileInfo)
// {
//     DownloadInfo downloadInfo(downloadID,
//                               m_url,
//                               m_currentHostKey,
//                               refFileInfo["name"].toString(),
//                               refFileInfo["path"].toString(),
//                               saveName,
//                               savePath,
//                               refFileInfo["size"].toLongLong(),
//                               0,
//                               refFileInfo["created"].toString(),
//                               refFileInfo["modified"].toString(),
//                               refFileInfo["accessed"].toString(),
//                               State::Active);

//     addDownloadToUnfinished(downloadInfo);

//     m_downloadInfoDict.emplace(downloadID, std::move(downloadInfo));
// }
