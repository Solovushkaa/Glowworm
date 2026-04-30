/**
 * @file download_manger.hpp
 * @brief -
 */

#ifndef DOWNLOADMANAGER_HPP
#define DOWNLOADMANAGER_HPP

#include <QFile>
#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include "download_info.hpp"
#include <type_traits>

/**
 * @brief -
 */
class DownloadManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Сonstructor for reading UnfinishedDownload.json file.
     */
    DownloadManager(
        const QString &savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/AppData/Client/UnfinishedDownloads.json");

    /**
     * @brief Read imcomlplit downloads from file.
     */
    bool readUnfinishedDownloads();

    void initInfo(DownloadInfo &downloadInfo, QJsonObject &jsonObject);

    void setDownloadInfoFromJsonObject(DownloadInfo &downloadInfo, QJsonObject &jsonObject);
    void setJsonObjectFromDownloadInfo(QJsonObject &jsonObject, DownloadInfo &downloadInfo);

    /**
     * @brief Adding download to a file and RAM
     */
    template<typename DInfo>
        requires std::same_as<std::remove_reference_t<DInfo>, DownloadInfo>
    bool addDownload(DInfo &&downloadInfo);
    /**
     * @brief Deleting downloads from a file and RAM
     */
    bool deleteDownload(const DownloadInfo &downloadInfo);

    /**
     * @brief Creating a list for qml
     */
    QList<QVariantHash> getListUIDataUnfinishedDownloads();
    /**
     * @brief Getting information about downloaded files
     */
    QHash<QString, DownloadInfo> &getDownloadInfoDict() { return m_downloadInfoDict; }

public slots:
    // void onDownloadAdded(QString &saveName, QString &downloadID, QVariantHash &refFileInfo);

private:
    QHash<QString, DownloadInfo> m_downloadInfoDict; ///< Information about downloaded files
    QJsonObject m_jsonUnfinishedDownloads;           ///< A JSON object to save to a file

    // Path to save imcomplited downloads
    const QString m_savePath;

public:
    using InfoType = DownloadInfo;
};

#endif // DOWNLOADMANAGER_HPP
