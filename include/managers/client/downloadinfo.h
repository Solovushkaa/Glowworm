/**
 * @file downloadinfo.h
 * @brief Represents a single file download task with metadata and state.
 *
 * This class encapsulates all information required to manage,
 * resume, and track a file download from a remote host.
 */

#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QObject>
#include <QString>
#include <string_view>
#include <QUrl>

struct BadMemberName
{
    constexpr const char *what() { return "Bad member name!"; }
};

namespace constants {

extern std::string_view DOWNLOADID;
extern std::string_view HOSTKEY;
extern std::string_view URL;
extern std::string_view NAME;
extern std::string_view PATH;
extern std::string_view SAVENAME;
extern std::string_view SAVEPATH;
extern std::string_view SIZE;
extern std::string_view LASTRECEIVEDBYTE;
extern std::string_view CREATED;
extern std::string_view MODIFIED;
extern std::string_view ACCESSED;
extern std::string_view DOWNLOADSTATUS;

} // namespace constants

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
const char *getDownloadInfoMemberName(DownloadInfoMember member);

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
