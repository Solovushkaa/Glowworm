#ifndef DIRECTCLIENT_H
#define DIRECTCLIENT_H

#include <QNetworkAccessManager>
#include <QObject>
#include "baseclient.h"
#include "downloadmanager.h"

class DirectClient : public BaseClient
{
    Q_OBJECT

public:
    DirectClient();

    void checkConnectionToServer(const QString &url) override;

    void getDirectoryList(const QString &path, const QString &url, const QString &userID) override;
    void getFile(QList<QVariantHash> &currentDirectory,
                 const QString &currentHostKey,
                 DownloadManager &downloadManager,
                 const QString &path,
                 const QString &savePath,
                 const QString &saveName) override;

    void startDownload(const QString &downloadID) override;
    void stopDownload(const QString &downloadID) override;

private slots:
    void onConnectionStatusReceived() override;
    void onDirectoryReceived() override;
    void onFileReceived() override;
    void onDownloadProgressed(qint64 bytesReceived, qint64 bytesTotal) override;

    /* Members */
private:
    QNetworkAccessManager *m_networkManager;
};

#endif // DIRECTCLIENT_H
