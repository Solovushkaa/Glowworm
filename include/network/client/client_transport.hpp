#ifndef CLIENTTRANSPORT_HPP
#define CLIENTTRANSPORT_HPP

#include <QObject>

class ClientTransport : public QObject
{
    Q_OBJECT

public:
    virtual ~ClientTransport() = default;

    virtual void startDownload(const QString &downloadID) = 0;
    virtual void stopDownload(const QString &downloadID) = 0;

    // virtual void getFile(QList<QVariantHash> &currentDirectory,
    //                      const QString &currentHostKey,
    //                      DownloadManager &downloadManager,
    //                      const QString &path,
    //                      const QString &savePath,
    //                      const QString &saveName)
    //     = 0;

public slots:
    virtual void onFileReceived() = 0;
    virtual void onDownloadProgressed(qint64 bytesReceived, qint64 bytesTotal) = 0;

signals:
    void newDownload(const QString &name, const QString &path, qint64 size);
    void downloadProgressChanged(const QString &downloadID, qint64 bytesReceived);
    void downloadStarted(const QString &name, const QString &path, qint64 size);
    void downloadStopped(const QString &downloadID, qint64 bytesReceived);
};

#endif // CLIENTTRANSPORT_HPP
