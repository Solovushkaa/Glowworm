#include "downloadinfo.hpp"

namespace constants {

constinit QStringView DOWNLOADID = u"DOWNLOADID";
constinit QStringView HOSTKEY = u"HOSTKEY";
constinit QStringView URL = u"URL";
constinit QStringView NAME = u"NAME";
constinit QStringView PATH = u"PATH";
constinit QStringView SAVENAME = u"SAVENAME";
constinit QStringView SAVEPATH = u"SAVEPATH";
constinit QStringView SIZE = u"SIZE";
constinit QStringView LASTRECEIVEDBYTE = u"LASTRECEIVEDBYTE";
constinit QStringView CREATED = u"CREATED";
constinit QStringView MODIFIED = u"MODIFIED";
constinit QStringView ACCESSED = u"ACCESSED";
constinit QStringView DOWNLOADSTATUS = u"DOWNLOADSTATUS";

} // namespace constants

const char *getDownloadInfoMemberName(DownloadInfoMember member)
{
    using enum DownloadInfoMember;

    switch (member) {
    case DOWNLOADID:
        return "downloadID";
    case HOSTKEY:
        return "hostKey";
    case URL:
        return "url";
    case NAME:
        return "name";
    case PATH:
        return "path";
    case SAVENAME:
        return "saveName";
    case SAVEPATH:
        return "savePath";
    case SIZE:
        return "size";
    case LASTRECEIVEDBYTE:
        return "lastReceivedByte";
    case CREATED:
        return "created";
    case MODIFIED:
        return "modified";
    case ACCESSED:
        return "accessed";
    case DOWNLOADSTATUS:
        return "downloadStatus";
    default:
        throw BadMemberName{};
    }
}

DownloadInfo::DownloadInfo(QObject *parent)
    : QObject(parent)
{}

DownloadInfo::DownloadInfo(const QString &downloadID,
                           const QUrl &URL,
                           const QString &hostKey,
                           const QString &fileName,
                           const QString &filePath,
                           const QString &fileSaveName,
                           const QString &fileSavePath,
                           qint64 fileSize,
                           qint64 fileLastReceivedByte,
                           const QString &fileDateCreated,
                           const QString &fileDateLastModified,
                           const QString &fileDateLastAccessed,
                           State downloadStatus)
    : m_downloadID(downloadID)
    , m_URL(URL)
    , m_hostKey(hostKey)
    , m_name(fileName)
    , m_path(filePath)
    , m_saveName(fileSaveName)
    , m_savePath(fileSavePath)
    , m_size(fileSize)
    , m_lastReceivedByte(fileLastReceivedByte)
    , m_created(fileDateCreated)
    , m_modified(fileDateLastModified)
    , m_accessed(fileDateLastAccessed)
    , m_downloadStatus(downloadStatus)
{}

DownloadInfo &DownloadInfo::operator=(const DownloadInfo &downloadInfo)
{
    m_downloadID = downloadInfo.m_downloadID;
    m_URL = downloadInfo.m_URL;
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
    m_downloadStatus = downloadInfo.m_downloadStatus;

    return *this;
}

DownloadInfo &DownloadInfo::operator=(DownloadInfo &&downloadInfo)
{
    m_downloadID = std::move(downloadInfo.m_downloadID);
    m_URL = std::move(downloadInfo.m_URL);
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
    m_downloadStatus = downloadInfo.m_downloadStatus;

    return *this;
}

void DownloadInfo::setDownloadStatus(const State newDownloadStatus)
{
    m_downloadStatus = newDownloadStatus;
    emit downloadStatusChanged();
}
