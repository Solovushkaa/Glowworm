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

void DownloadInfo::setDownloadStatus(State newDownloadStatus)
{
    m_downloadStatus = newDownloadStatus;
    emit downloadStatusChanged();
}
