/**
 * @file downloadinfo.hpp
 * @brief -
 */

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QFile>
#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include "downloadinfo.hpp"
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
        QString savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                           + "/AppData/Client/UnfinishedDownloads.json");

    /**
     * @brief Read imcomlplit downloads from file.
     */
    bool readUnfinishedDownloads();
    /**
     * @brief Checking the correctness of objects.
     */
    bool isCorrectDownloadInfoObject(const QJsonObject &object);

    bool rewriteFile(QStringView errorString, QStringView successString);
    void setDownloadInfoFromJson(DownloadInfo &downloadInfo, QJsonObject &object);
    void setJsonFromDownloadInfo(QJsonObject &object, DownloadInfo &downloadInfo);
    QJsonDocument parseJson(QByteArray &data);

    /**
     * @brief Adding download to a file and RAM
     */
    template<typename DInfo>
        requires std::same_as<std::remove_reference_t<DInfo>, DownloadInfo>
    bool addDownloadToUnfinished(DInfo &&downloadInfo);
    /**
     * @brief Deleting downloads from a file and RAM
     */
    bool deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo);

    /**
     * @brief Creating a list for qml
     */
    QList<QVariantHash> getVariantListUnfinishedDownloads();
    /**
     * @brief Getting information about downloaded files
     */
    QHash<QString, DownloadInfo> &getDownloadInfoDict() { return m_downloadInfoDict; }

public slots:
    // void onDownloadAdded(QString &saveName, QString &downloadID, QVariantHash &refFileInfo);

private:
    QHash<QString, DownloadInfo> m_downloadInfoDict; ///< Information about downloaded files
    QJsonObject m_jsonDownloadInfo;                  ///< A JSON object to save to a file

    // Path to save imcomplited downloads
    const QString m_savePath;
};

#endif // DOWNLOADMANAGER_H
