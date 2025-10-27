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
    UnfinishedDownloadManager();

    bool addDownloadToUnfinished(const DownloadInfo &downloadInfo);
    bool deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo);

    QHash<QString, DownloadInfo> readUnfinishedDownloads();
    QList<QVariantHash> getVariantListUnfinishedDownloads(
        const QHash<QString, DownloadInfo> &downloadInfo);

    QHash<QString, DownloadInfo> &getDownloadInfoDict() { return m_downloadInfoDict; }

    //QML
    QList<QVariantHash> getUnfinishedDownloads()
    {
        return getVariantListUnfinishedDownloads(m_downloadInfoDict);
    }

public slots:
    void onDownloadAdded(QString &saveName, QString &downloadID, QVariantHash &refFileInfo);

private:
    QHash<QString, DownloadInfo> m_downloadInfoDict; // Информация о каждом скачиваемом файле
};

#endif // UNFINISHEDDOWNLOADMANAGER_H
