#include "downloadinfo.h"

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
    , m_fileName(fileName)
    , m_filePath(filePath)
    , m_fileSaveName(fileSaveName)
    , m_fileSavePath(fileSavePath)
    , m_fileSize(fileSize)
    , m_fileLastReceivedByte(fileLastReceivedByte)
    , m_fileDateCreated(fileDateCreated)
    , m_fileDateLastModified(fileDateLastModified)
    , m_fileDateLastAccessed(fileDateLastAccessed)
    , m_downloadStatus(downloadStatus)
{}

DownloadInfo &DownloadInfo::operator=(const DownloadInfo &downloadInfo)
{
    m_downloadID = downloadInfo.m_downloadID;
    m_URL = downloadInfo.m_URL;
    m_hostKey = downloadInfo.m_hostKey;
    m_fileName = downloadInfo.m_fileName;
    m_filePath = downloadInfo.m_filePath;
    m_fileSaveName = downloadInfo.m_fileSaveName;
    m_fileSavePath = downloadInfo.m_fileSavePath;
    m_fileSize = downloadInfo.m_fileSize;
    m_fileLastReceivedByte = downloadInfo.m_fileLastReceivedByte;
    m_fileDateCreated = downloadInfo.m_fileDateCreated;
    m_fileDateLastModified = downloadInfo.m_fileDateLastModified;
    m_fileDateLastAccessed = downloadInfo.m_fileDateLastAccessed;
    m_downloadStatus = downloadInfo.m_downloadStatus;

    return *this;
}

DownloadInfo &DownloadInfo::operator=(DownloadInfo &&downloadInfo)
{
    m_downloadID = std::move(downloadInfo.m_downloadID);
    m_URL = std::move(downloadInfo.m_URL);
    m_hostKey = std::move(downloadInfo.m_hostKey);
    m_fileName = std::move(downloadInfo.m_fileName);
    m_filePath = std::move(downloadInfo.m_filePath);
    m_fileSaveName = std::move(downloadInfo.m_fileSaveName);
    m_fileSavePath = std::move(downloadInfo.m_fileSavePath);
    m_fileSize = downloadInfo.m_fileSize;
    m_fileLastReceivedByte = downloadInfo.m_fileLastReceivedByte;
    m_fileDateCreated = std::move(downloadInfo.m_fileDateCreated);
    m_fileDateLastModified = std::move(downloadInfo.m_fileDateLastModified);
    m_fileDateLastAccessed = std::move(downloadInfo.m_fileDateLastAccessed);
    m_downloadStatus = downloadInfo.m_downloadStatus;

    return *this;
}

void DownloadInfo::setDownloadStatus(State newDownloadStatus)
{
    m_downloadStatus = newDownloadStatus;
    emit downloadStatusChanged();
}
