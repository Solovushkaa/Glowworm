#include "download_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(download_manager, "download.manager")

DownloadManager::DownloadManager(const QString &savePath, QObject *parent)
    : QAbstractListModel(parent)
    , m_savePath(savePath)
{
    qCDebug(download_manager) << "DownloadManager - created";
}

DownloadManager::~DownloadManager()
{
    qCDebug(download_manager) << "DownloadManager - destroyed";
}

int DownloadManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        qCDebug(download_manager) << "QModelIndex parent is not valid";
        return 0;
    }
    return m_downloadInfoList.size();
}

QVariant DownloadManager::data(const QModelIndex &index, int role) const
{
    qCDebug(download_manager) << "Getting DownloadManager data for the QML List Model";

    if (!index.isValid() || index.row() >= m_downloadInfoList.size())
        return QVariant();

    DownloadInfo *info = m_downloadInfoList[index.row()];
    switch (role) {
    case DownloadIDRole:
        return info->m_downloadID;
    case UrlRole:
        return info->m_url;
    case HostKeyRole:
        return info->m_hostKey;
    case NameRole:
        return info->m_name;
    case PathRole:
        return info->m_path;
    case SaveNameRole:
        return info->m_saveName;
    case SavePathRole:
        return info->m_savePath;
    case SizeRole:
        return info->m_size;
    case LastReceivedByteRole:
        return info->m_lastReceivedByte;
    case CreatedRole:
        return info->m_created;
    case ModifiedRole:
        return info->m_modified;
    case AccessedRole:
        return info->m_accessed;
    case ConnectionStateRole:
        return static_cast<int>(info->m_downloadState);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> DownloadManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DownloadIDRole] = constants::kDownloadID.toUtf8();
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
    roles[ConnectionStateRole] = constants::kDownloadState.toUtf8();
    return roles;
}

bool DownloadManager::addDownload(DownloadInfo *downloadInfo)
{
    const int row = m_downloadInfoList.size();
    beginInsertRows(QModelIndex(), row, row);
    m_downloadInfoList.append(downloadInfo);
    endInsertRows();

    QJsonObject jsonObject;
    setJsonObjectFromDownloadInfo(jsonObject, downloadInfo);

    m_jsonUnfinishedDownloads.insert(downloadInfo->m_downloadID, std::move(jsonObject));
    m_downloadInfoDict[downloadInfo->m_downloadID] = downloadInfo;

    emit downloadAdded();

    return rewriteAppDataFile(m_savePath, m_jsonUnfinishedDownloads);
}

bool DownloadManager::addDownload(const QString &downloadID,
                                  const QUrl &url,
                                  const QString &hostKey,
                                  const QString &name,
                                  const QString &path,
                                  const QString &saveName,
                                  const QString &savePath,
                                  qint64 size,
                                  qint64 lastReceivedByte,
                                  const QString &created,
                                  const QString &modified,
                                  const QString &accessed,
                                  DownloadInfo::DownloadState downloadState)
{
    qCDebug(download_manager) << "Adding a new download";

    if (m_downloadInfoDict.contains(downloadID)) {
        qCInfo(download_manager) << "A download with the downloadID" << downloadID
                                 << "already exists";
        return false;
    }

    auto downloadInfo = new DownloadInfo(downloadID,
                                         url,
                                         hostKey,
                                         name,
                                         path,
                                         saveName,
                                         savePath,
                                         size,
                                         lastReceivedByte,
                                         created,
                                         modified,
                                         accessed,
                                         downloadState,
                                         this);

    return addDownload(downloadInfo);
}

bool DownloadManager::deleteDownload(int deleteIndex)
{
    qCDebug(download_manager) << "Deleting a download with an index";

    if (deleteIndex < 0 || deleteIndex >= m_downloadInfoList.size()) {
        qCCritical(download_manager) << "Bad DownloadInfo removal deleteIndex!";
        return false;
    }

    QString deleteID = std::move(m_downloadInfoList[deleteIndex]->m_downloadID);

    beginRemoveRows(QModelIndex(), deleteIndex, deleteIndex);
    DownloadInfo *info = m_downloadInfoList.takeAt(deleteIndex);
    info->deleteLater();
    endRemoveRows();

    m_downloadInfoDict.remove(deleteID);
    m_jsonUnfinishedDownloads.remove(deleteID);

    emit downloadRemoved(deleteIndex);

    return rewriteAppDataFile(m_savePath, m_jsonUnfinishedDownloads);
}

bool DownloadManager::deleteDownload(DownloadInfo *downloadInfo)
{
    qCDebug(download_manager) << "Deleting a download with an object";

    int deleteIndex = m_downloadInfoList.indexOf(downloadInfo);
    return deleteDownload(deleteIndex);
}

bool DownloadManager::updateDownload(int index, const QString &property, const QVariant &value)
{
    qCDebug(download_manager) << "Updating DownloadManager data for the QML List Model";

    if (index < 0 || index >= m_downloadInfoList.size()) {
        qCCritical(download_manager) << "Bad DownloadInfo update index!";
        return false;
    }

    DownloadInfo *info = m_downloadInfoList[index];

    info->setProperty(property.toUtf8().constData(), value);

    QVector<int> roles;
    if (property == constants::kDownloadID.toUtf8())
        roles << DownloadIDRole;
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
        roles << ConnectionStateRole;

    static const QVector<int> allRoles{DownloadIDRole,
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
                                       ConnectionStateRole};

    QModelIndex idx = this->index(index);

    emit dataChanged(idx, idx, roles.isEmpty() ? allRoles : roles);

    return true;
}

bool DownloadManager::readUnfinishedDownloads()
{
    return readAppData(*this, m_savePath, m_jsonUnfinishedDownloads);
}

void DownloadManager::initInfo(DownloadInfo *downloadInfo, QJsonObject &jsonObject)
{
    qCDebug(download_manager) << "Filling in download information from JSON";

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
    jsonObject[constants::kDownloadState] = static_cast<int>(downloadInfo->m_downloadState);
}
