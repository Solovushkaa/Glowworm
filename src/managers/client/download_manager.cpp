#include "download_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

DownloadManager::DownloadManager(const QString &savePath)
    : m_savePath(savePath)
{}

// TODO: Think about refactoring and behavior when reading the broken config
bool DownloadManager::readUnfinishedDownloads()
{
    return readPreset(*this, m_savePath, m_jsonUnfinishedDownloads);
}

void DownloadManager::initInfo(DownloadInfo &downloadInfo, QJsonObject &jsonObject)
{
    setDownloadInfoFromJsonObject(downloadInfo, jsonObject);

    m_downloadInfoDict.insert(downloadInfo.m_downloadID, std::move(downloadInfo));

    // Set the download state to "paused" on startup because the previous
    // session may have ended improperly without pausing.
    jsonObject[constants::kDownloadState] = static_cast<int>(DownloadState::Pause);
}

void DownloadManager::setDownloadInfoFromJsonObject(DownloadInfo &downloadInfo,
                                                    QJsonObject &jsonObject)
{
    downloadInfo.m_downloadID = jsonObject[constants::kDownloadID].toString();
    downloadInfo.m_hostKey = jsonObject[constants::kHostkey].toString();
    downloadInfo.m_URL = jsonObject[constants::kURL].toString();
    downloadInfo.m_name = jsonObject[constants::kName].toString();
    downloadInfo.m_path = jsonObject[constants::kPath].toString();
    downloadInfo.m_saveName = jsonObject[constants::kSaveName].toString();
    downloadInfo.m_savePath = jsonObject[constants::kSavePath].toString();
    downloadInfo.m_size = jsonObject[constants::kSize].toInteger();
    downloadInfo.m_lastReceivedByte = jsonObject[constants::kLastReceivedByte].toInteger();
    downloadInfo.m_created = jsonObject[constants::kCreated].toString();
    downloadInfo.m_modified = jsonObject[constants::kModified].toString();
    downloadInfo.m_accessed = jsonObject[constants::kAccessed].toString();
    downloadInfo.m_downloadState = DownloadState::Pause;
}

void DownloadManager::setJsonObjectFromDownloadInfo(QJsonObject &jsonObject,
                                                    DownloadInfo &downloadInfo)
{
    jsonObject[constants::kDownloadID] = downloadInfo.m_downloadID;
    jsonObject[constants::kURL] = downloadInfo.m_URL.toString();
    jsonObject[constants::kHostkey] = downloadInfo.m_hostKey;
    jsonObject[constants::kName] = downloadInfo.m_name;
    jsonObject[constants::kPath] = downloadInfo.m_path;
    jsonObject[constants::kSaveName] = downloadInfo.m_saveName;
    jsonObject[constants::kSavePath] = downloadInfo.m_savePath;
    jsonObject[constants::kSize] = downloadInfo.m_size;
    jsonObject[constants::kLastReceivedByte] = downloadInfo.m_lastReceivedByte;
    jsonObject[constants::kCreated] = downloadInfo.m_created;
    jsonObject[constants::kModified] = downloadInfo.m_modified;
    jsonObject[constants::kAccessed] = downloadInfo.m_accessed;
    jsonObject[constants::kDownloadState] = static_cast<int>(downloadInfo.getDownloadState());
}

template<typename DInfo>
    requires std::same_as<std::remove_reference_t<DInfo>, DownloadInfo>
bool DownloadManager::addDownload(DInfo &&downloadInfo)
{
    QString downloadName(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QJsonObject jsonObject;
    setJsonObjectFromDownloadInfo(jsonObject, downloadInfo);

    m_jsonUnfinishedDownloads.insert(downloadName, std::move(jsonObject));

    if constexpr (std::is_rvalue_reference_v<decltype(downloadInfo)>) {
        m_downloadInfoDict[downloadInfo.m_downloadID] = std::move(downloadInfo);
    } else {
        m_downloadInfoDict[downloadInfo.m_downloadID] = downloadInfo;
    }

    return rewriteConfigFile(m_savePath, m_jsonUnfinishedDownloads);
}
template bool DownloadManager::addDownload<DownloadInfo &>(DownloadInfo &);
template bool DownloadManager::addDownload<DownloadInfo &&>(DownloadInfo &&);

bool DownloadManager::deleteDownload(const DownloadInfo &downloadInfo)
{
    QString deleteID = std::move(downloadInfo.m_downloadID);

    m_downloadInfoDict.remove(deleteID);
    m_jsonUnfinishedDownloads.remove(deleteID);

    return rewriteConfigFile(m_savePath, m_jsonUnfinishedDownloads);
}

QList<QVariantHash> DownloadManager::getListUIDataUnfinishedDownloads()
{
    QList<QVariantHash> unfinishedDownloads;
    for (auto &downloadInfo : m_downloadInfoDict) {
        unfinishedDownloads.push_back(QVariantHash());
        unfinishedDownloads.back()[constants::kName] = downloadInfo.m_name;
        unfinishedDownloads.back()[constants::kDownloadID] = downloadInfo.m_downloadID;
        unfinishedDownloads.back()[constants::kSize] = downloadInfo.m_size;
        unfinishedDownloads.back()[constants::kLastReceivedByte] = downloadInfo.m_lastReceivedByte;
    }

    return unfinishedDownloads;
}
