#ifndef UNFINISHEDDOWNLOADMANAGER_H
#define UNFINISHEDDOWNLOADMANAGER_H

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include <downloadinfo.h>

class UnfinishedDownloadManager : public QObject
{
    Q_OBJECT

public:
    UnfinishedDownloadManager();

    //Read imcomlplit downloads from file
    bool readUnfinishedDownloads();
    //Checking the correctness of objects
    bool isCorrectDownloadInfoObject(const QJsonObject &object);

    // Adding download to a file and RAM
    bool addDownloadToUnfinished(DownloadInfo &downloadInfo);
    // Deleting downloads from a file and RAM
    bool deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo);

    // Creating a list for qml
    QList<QVariantHash> getVariantListUnfinishedDownloads();
    // Getting information about downloaded files
    QHash<QString, DownloadInfo> &getDownloadInfoDict() { return m_downloadInfoDict; }

public slots:
    // void onDownloadAdded(QString &saveName, QString &downloadID, QVariantHash &refFileInfo);

private:
    QHash<QString, DownloadInfo> m_downloadInfoDict; // Information about downloaded files
    QJsonObject m_jsonDownloadInfo;                  // A JSON object to save to a file

    // All fields of information about downloads
    const QList<const char *> m_checkList = {"downloadID",
                                             "hostKey",
                                             "url",
                                             "fileName",
                                             "filePath",
                                             "fileSaveName",
                                             "fileSavePath",
                                             "fileSize",
                                             "fileLastReceivedByte",
                                             "fileDateCreated",
                                             "fileDateLastModified",
                                             "fileDateLastAccessed"};

    const QString m_savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                               + "/AppData/Client/UnfinishedDownloads.json";
};

#endif // UNFINISHEDDOWNLOADMANAGER_H
