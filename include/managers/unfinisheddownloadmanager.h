#ifndef UNFINISHEDDOWNLOADMANAGER_H
#define UNFINISHEDDOWNLOADMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <downloadinfo.h>

class UnfinishedDownloadManager : public QObject
{
    Q_OBJECT
    QML_SINGLETON

public:
    explicit UnfinishedDownloadManager();

    bool addDownloadToUnfinished(const DownloadInfo &downloadInfo);
    bool deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo);

    QHash<QString, DownloadInfo> readUnfinishedDownloads();
    QList<QVariantHash> getVariantListUnfinishedDownloads(
        const QHash<QString, DownloadInfo> &downloadInfo);
};

#endif // UNFINISHEDDOWNLOADMANAGER_H
