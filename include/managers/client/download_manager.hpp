/**
 * @file download_manger.hpp
 * @brief -
 */

#ifndef DOWNLOADMANAGER_HPP
#define DOWNLOADMANAGER_HPP

#include <QAbstractListModel>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include "download_info.hpp"
// #include <type_traits>

/**
 * @brief -
 */
class DownloadManager : public QAbstractListModel
{
    Q_OBJECT
    QML_UNCREATABLE("Managed from C++")

    enum DownloadRoles {
        DownloadIDRole = Qt::UserRole + 1,
        UrlRole,
        HostKeyRole,
        NameRole,
        PathRole,
        SaveNameRole,
        SavePathRole,
        SizeRole,
        LastReceivedByteRole,
        CreatedRole,
        ModifiedRole,
        AccessedRole,
        StateRole
    };

    // --- ModelView methods ---
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Adding download
     */
private:
    bool addDownload(DownloadInfo *info);

public:
    bool addDownload(const QString &downloadID,
                     const QUrl &url,
                     const QString &hostKey,
                     const QString &name,
                     const QString &path,
                     const QString &saveName,
                     const QString &savePath,
                     qint64 size,
                     qint64 lastReceivedByte,
                     const QString &created,
                     const QString &modified,
                     const QString &accessed,
                     DownloadInfo::DownloadState downloadState);
    /**
     * @brief Removing download
     */
    Q_INVOKABLE bool removeDownload(int index);
    bool removeDownload(DownloadInfo *downloadInfo);
    Q_INVOKABLE bool updateDownload(int index, const QString &property, const QVariant &value);

public:
    /**
     * @brief Сonstructor for reading UnfinishedDownload.json file.
     */
    DownloadManager(
        const QString &savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/AppData/Client/UnfinishedDownloads.json",
        QObject *parent = nullptr);

    /**
     * @brief Read imcomlplit downloads from file.
     */
    bool readUnfinishedDownloads();

    void initInfo(DownloadInfo *downloadInfo, QJsonObject &jsonObject);

    void setDownloadInfoFromJsonObject(DownloadInfo *downloadInfo, QJsonObject &jsonObject);
    void setJsonObjectFromDownloadInfo(QJsonObject &jsonObject, DownloadInfo *downloadInfo);

    /**
     * @brief Getting information about downloaded files
     */
    QHash<QString, DownloadInfo *> &getDownloadInfoDict() { return m_downloadInfoDict; }
    QList<DownloadInfo *> &getDownloadInfoList() { return m_downloadInfoList; }

signals:
    void downloadAdded();
    void downloadRemoved(int index);

private:
    QList<DownloadInfo *> m_downloadInfoList;          ///< Owns DownloadInfo objects
    QHash<QString, DownloadInfo *> m_downloadInfoDict; ///< Information about downloaded files
    QJsonObject m_jsonUnfinishedDownloads;           ///< A JSON object to save to a file

    const QString m_savePath; ///< Path to save unfinished downloads

public:
    using InfoType = DownloadInfo;
};

#endif // DOWNLOADMANAGER_HPP
