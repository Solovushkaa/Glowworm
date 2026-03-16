/**
 * @file downloadinfo.h
 * @brief Represents a single file download task with metadata and state.
 *
 * This class encapsulates all information required to manage,
 * resume, and track a file download from a remote host.
 */

#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QHash>
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
enum class State {
    Pause,  ///< Download is paused
    Wait,   ///< Task is queued but not started
    Active, ///< Data is being transferred
    Finish, ///< Download finished successfully
    Error   ///< Download failed due to error
};

namespace constants {

constinit const char *DOWNLOADID = "DOWNLOADID";
constinit const char *HOSTKEY = "HOSTKEY";
constinit const char *URL = "URL";
constinit const char *NAME = "NAME";
constinit const char *PATH = "PATH";
constinit const char *SAVENAME = "SAVENAME";
constinit const char *SAVEPATH = "SAVEPATH";
constinit const char *SIZE = "SIZE";
constinit const char *LASTRECEIVEDBYTE = "LASTRECEIVEDBYTE";
constinit const char *CREATED = "CREATED";
constinit const char *MODIFIED = "MODIFIED";
constinit const char *ACCESSED = "ACCESSED";
constinit const char *DOWNLOADSTATUS = "DOWNLOADSTATUS";

} // namespace constants

/**
 * @brief All fields of information about downloads.
 */
enum class DownloadInfoMember {
    DOWNLOADID,
    HOSTKEY,
    URL,
    NAME,
    PATH,
    SAVENAME,
    SAVEPATH,
    SIZE,
    LASTRECEIVEDBYTE,
    CREATED,
    MODIFIED,
    ACCESSED,
    DOWNLOADSTATUS,
    COUNT // To get the size of the enum
};

/**
 * @brief All fields of information about downloads.
 * @param member Enum
 * @return Enum type converted into const char *
 */
const char *getDownloadInfoMemberName(DownloadInfoMember member)
{
    using enum DownloadInfoMember;

    switch (member) {
    case DOWNLOADID:
        return "downloadID";
    case HOSTKEY:
        return "hostKey";
    case URL:
        return "url";
    case NAME:
        return "name";
    case PATH:
        return "path";
    case SAVENAME:
        return "saveName";
    case SAVEPATH:
        return "savePath";
    case SIZE:
        return "size";
    case LASTRECEIVEDBYTE:
        return "lastReceivedByte";
    case CREATED:
        return "created";
    case MODIFIED:
        return "modified";
    case ACCESSED:
        return "accessed";
    case DOWNLOADSTATUS:
        return "downloadStatus";
    default:
        throw BadMemberName{};
    }
}

/**
 * @struct DownloadInfo
 * @brief Manages metadata and state for a single file download.
 */
struct DownloadInfo : public QObject
{
    Q_OBJECT

    // --- Methods ---
public:
    /**
     * @brief Sets a new download status.
     */
    void setDownloadStatus(const State newDownloadStatus)
    {
        m_downloadStatus = newDownloadStatus;
        emit downloadStatusChanged();
    }
    /**
     * @brief Provides download status.
     */
    State getDownloadStatus() const { return m_downloadStatus; }

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
    State m_downloadStatus;         ///< Download status

    // --- Signals ---
signals:
    /**
     * @brief Notifies about download status change.
     */
    void downloadStatusChanged();
};

#endif // DOWNLOADINFO_H
