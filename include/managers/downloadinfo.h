#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QObject>
#include <QString>
#include <QUrl>

enum class State { Pause, Waiting, Active, Finished, Error };

struct DownloadInfo : public QObject
{
    Q_OBJECT

    /* Constructors */
public:
    explicit DownloadInfo(QObject *parent = nullptr);
    DownloadInfo(const QString &downloadID,
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
                 State downloadStatus);
    DownloadInfo(const DownloadInfo &downloadInfo) { *this = downloadInfo; }
    DownloadInfo &operator=(const DownloadInfo &downloadInfo)
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
        return *this;
    }
    DownloadInfo(DownloadInfo &&downloadInfo) { *this = std::move(downloadInfo); }
    DownloadInfo &operator=(DownloadInfo &&downloadInfo)
    {
        m_downloadID = std::move(downloadInfo.m_downloadID);
        m_URL = std::move(downloadInfo.m_URL);
        m_hostKey = std::move(downloadInfo.m_hostKey);
        m_fileName = std::move(downloadInfo.m_fileName);
        m_filePath = std::move(downloadInfo.m_filePath);
        m_fileSaveName = std::move(downloadInfo.m_fileSaveName);
        m_fileSavePath = std::move(downloadInfo.m_fileSavePath);
        m_fileSize = std::move(downloadInfo.m_fileSize);
        m_fileLastReceivedByte = std::move(downloadInfo.m_fileLastReceivedByte);
        m_fileDateCreated = std::move(downloadInfo.m_fileDateCreated);
        m_fileDateLastModified = std::move(downloadInfo.m_fileDateLastModified);
        m_fileDateLastAccessed = std::move(downloadInfo.m_fileDateLastAccessed);
        return *this;
    }

    /* Methods */
public:
    void setDownloadStatus(State newDownloadStatus);
    State getDownloadStatus() const { return m_downloadStatus; }

    /* Members */
public:
    QString m_downloadID;               // id загрузки
    QUrl m_URL;                         // url хоста
    QString m_hostKey;                  // ключ хоста
    QString m_fileName;                 // имя файла
    QString m_filePath;                 // путь до файла на хосте
    QString m_fileSaveName;             // имя для сохранения скачиваемого файла
    QString m_fileSavePath;             // путь до места сохранения файла
    qint64 m_fileSize;                  // размер файла
    qint64 m_fileLastReceivedByte;      // начальная позиция в файле(в байтах)
    QString m_fileDateCreated;          // время создания файла
    QString m_fileDateLastModified;     // время последнего имзменения
    QString m_fileDateLastAccessed;     // время последнего использования файла

private:
    State m_downloadStatus; // статус установки

    /* Signals */
signals:
    void downloadStatusChanged();
};

#endif // DOWNLOADINFO_H
