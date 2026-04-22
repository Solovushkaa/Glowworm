#include "downloadinfo.hpp"

namespace constants {

constinit QStringView DOWNLOADID = u"downloadID";
constinit QStringView HOSTKEY = u"hostKey";
constinit QStringView URL = u"url";
constinit QStringView NAME = u"name";
constinit QStringView PATH = u"path";
constinit QStringView SAVENAME = u"saveName";
constinit QStringView SAVEPATH = u"savePath";
constinit QStringView SIZE = u"size";
constinit QStringView LASTRECEIVEDBYTE = u"lastReceivedByte";
constinit QStringView CREATED = u"created";
constinit QStringView MODIFIED = u"modified";
constinit QStringView ACCESSED = u"accessed";
constinit QStringView DOWNLOADSTATUS = u"downloadStatus";

} // namespace constants

QStringView getDownloadInfoMemberName(DownloadInfoMember member)
{
    using enum DownloadInfoMember;

    switch (member) {
    case DOWNLOADID:
        return constants::DOWNLOADID;
    case HOSTKEY:
        return constants::HOSTKEY;
    case URL:
        return constants::URL;
    case NAME:
        return constants::NAME;
    case PATH:
        return constants::PATH;
    case SAVENAME:
        return constants::SAVENAME;
    case SAVEPATH:
        return constants::SAVEPATH;
    case SIZE:
        return constants::SIZE;
    case LASTRECEIVEDBYTE:
        return constants::LASTRECEIVEDBYTE;
    case CREATED:
        return constants::CREATED;
    case MODIFIED:
        return constants::MODIFIED;
    case ACCESSED:
        return constants::ACCESSED;
    case DOWNLOADSTATUS:
        return constants::DOWNLOADSTATUS;
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
