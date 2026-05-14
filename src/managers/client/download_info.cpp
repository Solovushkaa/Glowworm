#include "download_info.hpp"
#include "constants.hpp"
#include <stdexcept>

Q_LOGGING_CATEGORY(download_info, "download.info")

namespace downloads {

QStringView getDownloadInfoMemberName(DownloadInfoMember member)
{
    using enum DownloadInfoMember;

    switch (member) {
    case DownloadID:
        return constants::kDownloadID;
    case Hostkey:
        return constants::kHostkey;
    case URL:
        return constants::kURL;
    case Name:
        return constants::kName;
    case Path:
        return constants::kPath;
    case SaveName:
        return constants::kSaveName;
    case SavePath:
        return constants::kSavePath;
    case Size:
        return constants::kSize;
    case LastReceivedByte:
        return constants::kLastReceivedByte;
    case Created:
        return constants::kCreated;
    case Modified:
        return constants::kModified;
    case Accessed:
        return constants::kAccessed;
    case DownloadState:
        return constants::kDownloadState;
    default:
        throw std::invalid_argument("Unknown DownloadInfoMember to convert to string");
    }
}

} // namespace downloads

DownloadInfo::DownloadInfo(QObject *parent)
    : QObject(parent)
{
    qCDebug(download_info) << "DownloadInfo empty object successfully created";
}

DownloadInfo::DownloadInfo(const QString &downloadID,
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
                           DownloadInfo::DownloadState downloadState,
                           QObject *parent)
    : QObject(parent)
    , m_downloadID(downloadID)
    , m_url(url)
    , m_hostKey(hostKey)
    , m_name(name)
    , m_path(path)
    , m_saveName(saveName)
    , m_savePath(savePath)
    , m_size(size)
    , m_lastReceivedByte(lastReceivedByte)
    , m_created(created)
    , m_modified(modified)
    , m_accessed(accessed)
    , m_downloadState(downloadState)
{
    qCDebug(download_info) << "DownloadInfo:" << m_name << "- created";
}

DownloadInfo::DownloadInfo(const DownloadInfo &downloadInfo)
{
    *this = downloadInfo;
}

DownloadInfo &DownloadInfo::operator=(const DownloadInfo &downloadInfo)
{
    m_downloadID = downloadInfo.m_downloadID;
    m_url = downloadInfo.m_url;
    m_hostKey = downloadInfo.m_hostKey;
    m_name = downloadInfo.m_name;
    m_path = downloadInfo.m_path;
    m_saveName = downloadInfo.m_saveName;
    m_savePath = downloadInfo.m_savePath;
    m_size = downloadInfo.m_size;
    m_lastReceivedByte = downloadInfo.m_lastReceivedByte;
    m_created = downloadInfo.m_created;
    m_modified = downloadInfo.m_modified;
    m_accessed = downloadInfo.m_accessed;
    m_downloadState = downloadInfo.m_downloadState;
    this->setParent(downloadInfo.parent());

    qCDebug(download_info) << "DownloadInfo:" << m_name << "- copyed";

    return *this;
}

DownloadInfo::DownloadInfo(DownloadInfo &&downloadInfo)
{
    *this = std::move(downloadInfo);
}

DownloadInfo &DownloadInfo::operator=(DownloadInfo &&downloadInfo)
{
    m_downloadID = std::move(downloadInfo.m_downloadID);
    m_url = std::move(downloadInfo.m_url);
    m_hostKey = std::move(downloadInfo.m_hostKey);
    m_name = std::move(downloadInfo.m_name);
    m_path = std::move(downloadInfo.m_path);
    m_saveName = std::move(downloadInfo.m_saveName);
    m_savePath = std::move(downloadInfo.m_savePath);
    m_size = downloadInfo.m_size;
    m_lastReceivedByte = downloadInfo.m_lastReceivedByte;
    m_created = std::move(downloadInfo.m_created);
    m_modified = std::move(downloadInfo.m_modified);
    m_accessed = std::move(downloadInfo.m_accessed);
    m_downloadState = downloadInfo.m_downloadState;
    this->setParent(downloadInfo.parent());

    qCDebug(download_info) << "DownloadInfo:" << m_name << "- moved";

    return *this;
}

DownloadInfo::~DownloadInfo()
{
    qCDebug(download_info) << "DownloadInfo:" << m_name << "- destroyed";
}
