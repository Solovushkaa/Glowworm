#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <QObject>
#include "downloadmanager.h"

class BaseClient : public QObject
{
    Q_OBJECT
public:
    BaseClient() = default;
    virtual ~BaseClient() = 0;

    virtual void checkConnectionToServer(const QString &url) = 0;

    virtual void getDirectoryList(const QString &path, const QString &url, const QString &userID) = 0;
    virtual void getFile(QList<QVariantHash> &currentDirectory,
                         const QString &currentHostKey,
                         DownloadManager &downloadManager,
                         const QString &path,
                         const QString &savePath,
                         const QString &saveName)
        = 0;

    virtual void startDownload(const QString &downloadID) = 0;
    virtual void stopDownload(const QString &downloadID) = 0;

signals:
    void requestFinished();
    void requestError(const QString &error);

    void newDownload(const QString &name, const QString &path, qint64 size);
    void downloadProgressChanged(const QString &downloadID, qint64 bytesReceived);
    void downloadStarted(const QString &name, const QString &path, qint64 size);
    void downloadStopped(const QString &downloadID, qint64 bytesReceived);
    void statusCodeChanged(int currentStatusCode);
    void currentHostChanged(QString &&hostKey);
    void currentDirectoryChanged(QList<QVariantHash> &&currentDirectory);

protected slots:
    virtual void onConnectionStatusReceived() = 0;
    virtual void onDirectoryReceived() = 0;
    virtual void onFileReceived() = 0;
    virtual void onDownloadProgressed(qint64 bytesReceived, qint64 bytesTotal) = 0;
};

#endif // BASECLIENT_H
