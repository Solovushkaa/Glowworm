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
    DownloadInfo &operator=(const DownloadInfo &downloadInfo);

    DownloadInfo(DownloadInfo &&downloadInfo) { *this = std::move(downloadInfo); }
    DownloadInfo &operator=(DownloadInfo &&downloadInfo);

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
    State m_downloadStatus;             // статус установки

    /* Signals */
signals:
    void downloadStatusChanged();
};

#endif // DOWNLOADINFO_H
