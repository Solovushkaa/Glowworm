#include "download_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

int DownloadManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_downloadInfoList.size();
}

QVariant DownloadManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_downloadInfoList.size())
        return QVariant();

    DownloadInfo *info = m_downloadInfoList.at(index.row());
    switch (role) {
    case DownloadRole:
        return info->downloadID();
    case UrlRole:
        return info->url();
    case HostKeyRole:
        return info->hostKey();
    case NameRole:
        return info->name();
    case PathRole:
        return info->path();
    case SaveNameRole:
        return info->saveName();
    case SavePathRole:
        return info->savePath();
    case SizeRole:
        return info->size();
    case LastReceivedByteRole:
        return info->lastReceivedByte();
    case CreatedRole:
        return info->created();
    case ModifiedRole:
        return info->modified();
    case AccessedRole:
        return info->accessed();
    case StateRole:
        return static_cast<int>(info->downloadState());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> DownloadManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DownloadRole] = constants::kDownloadID.toUtf8();
    roles[UrlRole] = constants::kURL.toUtf8();
    roles[HostKeyRole] = constants::kHostkey.toUtf8();
    roles[NameRole] = constants::kName.toUtf8();
    roles[PathRole] = constants::kPath.toUtf8();
    roles[SaveNameRole] = constants::kSaveName.toUtf8();
    roles[SavePathRole] = constants::kSavePath.toUtf8();
    roles[SizeRole] = constants::kSize.toUtf8();
    roles[LastReceivedByteRole] = constants::kLastReceivedByte.toUtf8();
    roles[CreatedRole] = constants::kCreated.toUtf8();
    roles[ModifiedRole] = constants::kModified.toUtf8();
    roles[AccessedRole] = constants::kAccessed.toUtf8();
    roles[StateRole] = constants::kDownloadState.toUtf8();
    return roles;
}

bool DownloadManager::addDownload(DownloadInfo *downloadInfo)
{
    if (downloadInfo == nullptr) {
        qCritical() << "DownloadInfo * canno't nullptr";
        return false;
    }

    downloadInfo->setParent(this);

    const int row = m_downloadInfoList.size();
    beginInsertRows(QModelIndex(), row, row);
    m_downloadInfoList.append(downloadInfo);
    endInsertRows();

    QJsonObject jsonObject;
    setJsonObjectFromDownloadInfo(jsonObject, downloadInfo);

    m_jsonUnfinishedDownloads.insert(downloadInfo->m_downloadID, std::move(jsonObject));
    m_downloadInfoDict[downloadInfo->m_downloadID] = downloadInfo;

    emit downloadAdded();

    return rewriteConfigFile(m_savePath, m_jsonUnfinishedDownloads);
}

bool DownloadManager::removeDownload(int index)
{
    if (index < 0 || index >= m_downloadInfoList.size()) {
        qCritical() << "Bad DownloadInfo removal index!";
        return false;
    }

    QString deleteID = std::move(m_downloadInfoList[index]->m_downloadID);

    beginRemoveRows(QModelIndex(), index, index);
    DownloadInfo *info = m_downloadInfoList.takeAt(index);
    info->deleteLater();
    endRemoveRows();

    m_downloadInfoDict.remove(deleteID);
    m_jsonUnfinishedDownloads.remove(deleteID);

    emit downloadRemoved(index);

    return rewriteConfigFile(m_savePath, m_jsonUnfinishedDownloads);
}

bool DownloadManager::removeDownload(DownloadInfo *downloadInfo)
{
    int index = m_downloadInfoList.indexOf(downloadInfo);
    QString deleteID = std::move(downloadInfo->m_downloadID);

    beginRemoveRows(QModelIndex(), index, index);
    DownloadInfo *info = m_downloadInfoList.takeAt(index);
    info->deleteLater();
    endRemoveRows();

    m_downloadInfoDict.remove(deleteID);
    m_jsonUnfinishedDownloads.remove(deleteID);

    emit downloadRemoved(index);

    return rewriteConfigFile(m_savePath, m_jsonUnfinishedDownloads);
}

bool DownloadManager::updateDownload(int index, const QString &property, const QVariant &value)
{
    if (index < 0 || index >= m_downloadInfoList.size()) {
        qCritical() << "Bad DownloadInfo update index!";
        return false;
    }

    DownloadInfo *info = m_downloadInfoList[index];

    info->setProperty(property.toUtf8().constData(), value);

    QVector<int> roles;
    if (property == constants::kDownloadID.toUtf8())
        roles << DownloadRole;
    if (property == constants::kURL.toUtf8())
        roles << UrlRole;
    if (property == constants::kHostkey.toUtf8())
        roles << HostKeyRole;
    if (property == constants::kName.toUtf8())
        roles << NameRole;
    if (property == constants::kPath.toUtf8())
        roles << PathRole;
    if (property == constants::kSaveName.toUtf8())
        roles << SaveNameRole;
    if (property == constants::kSavePath.toUtf8())
        roles << SavePathRole;
    if (property == constants::kSize.toUtf8())
        roles << SizeRole;
    if (property == constants::kLastReceivedByte.toUtf8())
        roles << LastReceivedByteRole;
    if (property == constants::kCreated.toUtf8())
        roles << CreatedRole;
    if (property == constants::kModified.toUtf8())
        roles << ModifiedRole;
    if (property == constants::kAccessed.toUtf8())
        roles << AccessedRole;
    if (property == constants::kDownloadState.toUtf8())
        roles << StateRole;

    static const QVector<int> allRoles{DownloadRole,
                                       UrlRole,
                                       HostKeyRole,
                                       NameRole,
                                       PathRole,
                                       SaveNameRole,
                                       SavePathRole,
                                       SizeRole,
                                       LastReceivedByteRole,
                                       CreatedRole,
                                       ModifiedRole,
                                       AccessedRole,
                                       StateRole};

    QModelIndex idx = this->index(index);

    emit dataChanged(idx, idx, roles.isEmpty() ? allRoles : roles);

    return true;
}

DownloadManager::DownloadManager(const QString &savePath, QObject *parent)
    : m_savePath(savePath)
    , QAbstractListModel(parent)
{}

bool DownloadManager::readUnfinishedDownloads()
{
    return readPreset(*this, m_savePath, m_jsonUnfinishedDownloads);
}

void DownloadManager::initInfo(DownloadInfo *downloadInfo, QJsonObject &jsonObject)
{
    downloadInfo->setParent(this);

    setDownloadInfoFromJsonObject(downloadInfo, jsonObject);

    m_downloadInfoList.push_back(downloadInfo);
    m_downloadInfoDict.insert(downloadInfo->m_downloadID, downloadInfo);

    // Set the download state to "paused" on startup because the previous
    // session may have ended improperly without pausing.
    jsonObject[constants::kDownloadState] = static_cast<int>(DownloadInfo::DownloadState::Pause);
}

void DownloadManager::setDownloadInfoFromJsonObject(DownloadInfo *downloadInfo,
                                                    QJsonObject &jsonObject)
{
    downloadInfo->m_downloadID = jsonObject[constants::kDownloadID].toString();
    downloadInfo->m_hostKey = jsonObject[constants::kHostkey].toString();
    downloadInfo->m_url = jsonObject[constants::kURL].toString();
    downloadInfo->m_name = jsonObject[constants::kName].toString();
    downloadInfo->m_path = jsonObject[constants::kPath].toString();
    downloadInfo->m_saveName = jsonObject[constants::kSaveName].toString();
    downloadInfo->m_savePath = jsonObject[constants::kSavePath].toString();
    downloadInfo->m_size = jsonObject[constants::kSize].toInteger();
    downloadInfo->m_lastReceivedByte = jsonObject[constants::kLastReceivedByte].toInteger();
    downloadInfo->m_created = jsonObject[constants::kCreated].toString();
    downloadInfo->m_modified = jsonObject[constants::kModified].toString();
    downloadInfo->m_accessed = jsonObject[constants::kAccessed].toString();
    downloadInfo->m_downloadState = DownloadInfo::DownloadState::Pause;
}

void DownloadManager::setJsonObjectFromDownloadInfo(QJsonObject &jsonObject,
                                                    DownloadInfo *downloadInfo)
{
    jsonObject[constants::kDownloadID] = downloadInfo->m_downloadID;
    jsonObject[constants::kURL] = downloadInfo->m_url.toString();
    jsonObject[constants::kHostkey] = downloadInfo->m_hostKey;
    jsonObject[constants::kName] = downloadInfo->m_name;
    jsonObject[constants::kPath] = downloadInfo->m_path;
    jsonObject[constants::kSaveName] = downloadInfo->m_saveName;
    jsonObject[constants::kSavePath] = downloadInfo->m_savePath;
    jsonObject[constants::kSize] = downloadInfo->m_size;
    jsonObject[constants::kLastReceivedByte] = downloadInfo->m_lastReceivedByte;
    jsonObject[constants::kCreated] = downloadInfo->m_created;
    jsonObject[constants::kModified] = downloadInfo->m_modified;
    jsonObject[constants::kAccessed] = downloadInfo->m_accessed;
    jsonObject[constants::kDownloadState] = static_cast<int>(downloadInfo->downloadState());
}
