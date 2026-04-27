/**
 * @file downloadinfo.hpp
 * @brief Represents a single file download task with metadata and state.
 *
 * This class encapsulates all information required to manage,
 * resume, and track a file download from a remote host.
 */

#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QObject>
#include <QString>
#include <QStringView>
#include <QUrl>

struct BadMemberName
{
    constexpr const char *what() { return "Bad member name!"; }
};

namespace constants {

inline constexpr QStringView DOWNLOADID = u"downloadID";
inline constexpr QStringView HOSTKEY = u"hostKey";
inline constexpr QStringView URL = u"url";
inline constexpr QStringView NAME = u"name";
inline constexpr QStringView PATH = u"path";
inline constexpr QStringView SAVENAME = u"saveName";
inline constexpr QStringView SAVEPATH = u"savePath";
inline constexpr QStringView SIZE = u"size";
inline constexpr QStringView LASTRECEIVEDBYTE = u"lastReceivedByte";
inline constexpr QStringView CREATED = u"created";
inline constexpr QStringView MODIFIED = u"modified";
inline constexpr QStringView ACCESSED = u"accessed";
inline constexpr QStringView DOWNLOADSTATUS = u"downloadStatus";

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
                 const QString &fileName,
                 const QString &filePath,
                 const QString &fileSaveName,
                 const QString &fileSavePath,
                 qint64 fileSize,
                 qint64 fileLastReceivedByte,
                 const QString &fileDateCreated,
                 const QString &fileDateLastModified,
                 const QString &fileDateLastAccessed,
                 State downloadStatus);

    DownloadInfo(const DownloadInfo &downloadInfo) { *this = downloadInfo; }
    DownloadInfo &operator=(const DownloadInfo &downloadInfo);

    DownloadInfo(DownloadInfo &&downloadInfo) { *this = std::move(downloadInfo); }
    DownloadInfo &operator=(DownloadInfo &&downloadInfo);

    // --- Methods ---
public:
    /**
     * @brief Sets a new download status.
     */
    void setDownloadStatus(const State newDownloadStatus);

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
