/**
 * @file download_info.hpp
 * @brief Represents a single file download task with metadata and state.
 *
 * This class encapsulates all information required to manage,
 * resume, and track a file download from a remote host.
 */

#ifndef DOWNLOADINFO_HPP
#define DOWNLOADINFO_HPP

#include <QObject>
#include <QString>
#include <QUrl>

struct BadMemberName
{
    constexpr const char *what() { return "Bad member name!"; }
};

/**
 * @enum State
 * @brief Represents the state of a download task.
 */
enum class DownloadState {
    Pause,  ///< Download is paused
    Wait,   ///< Task is queued but not started
    Active, ///< Data is being transferred
    Finish, ///< Download finished successfully
    Error   ///< Download failed due to error
};

/**
 * @brief All fields of information about downloads.
 */
enum class DownloadInfoMember {
    DownloadID,
    Hostkey,
    URL,
    Name,
    Path,
    SaveName,
    SavePath,
    Size,
    LastReceivedByte,
    Created,
    Modified,
    Accessed,
    DownloadState,
    COUNT // To get the size of the enum
};

/**
 * @brief All fields of information about downloads.
 * @param member Enum
 * @return Enum type converted into QStringView
 */
QStringView getDownloadInfoMemberName(DownloadInfoMember member);

/**
 * @struct DownloadInfo
 * @brief Manages metadata and state for a single file download.
 */
struct DownloadInfo : public QObject
{
    Q_OBJECT

    // --- Constructors ---
public:
    explicit DownloadInfo(QObject *parent = nullptr);
    DownloadInfo(const QString &downloadID,
                 const QUrl &URL,
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
                 DownloadState downloadState);

    DownloadInfo(const DownloadInfo &downloadInfo) { *this = downloadInfo; }
    DownloadInfo &operator=(const DownloadInfo &downloadInfo);

    DownloadInfo(DownloadInfo &&downloadInfo) { *this = std::move(downloadInfo); }
    DownloadInfo &operator=(DownloadInfo &&downloadInfo);

    // --- Methods ---
public:
    /**
     * @brief Sets a new download status.
     */
    void setDownloadState(const DownloadState newDownloadStatus);

    /**
     * @brief Provides download status.
     */
    DownloadState getDownloadState() const { return m_downloadState; }

    // --- Members ---
public:
    QString m_downloadID;           ///< Download ID
    QUrl m_URL;                     ///< Host URL
    QString m_hostKey;              ///< Host key
    QString m_name;                 ///< File name
    QString m_path;                 ///< File path on the host
    QString m_saveName;             ///< Name for saving the downloaded file
    QString m_savePath;             ///< Path to the file save location
    qint64 m_size;                  ///< File size
    qint64 m_lastReceivedByte;      ///< Starting position in the file (in bytes)
    QString m_created;              ///< File creation time
    QString m_modified;             ///< File last modification time
    QString m_accessed;             ///< File last access time
    DownloadState m_downloadState;  ///< Download status

    // --- Signals ---
signals:
    /**
     * @brief Notifies about download state change.
     */
    void downloadStateChanged();
};

#endif // DOWNLOADINFO_HPP
