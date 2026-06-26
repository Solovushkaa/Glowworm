/**
 * @file download_info.hpp
 * @brief Represents a single file download task with metadata and state.
 *
 * This class encapsulates all information required to manage,
 * resume, and track a file download from a remote host.
 */

#ifndef DOWNLOADINFO_HPP
#define DOWNLOADINFO_HPP

#include <QLoggingCategory>
#include <QObject>
#include <QProperty>
#include <QString>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

Q_DECLARE_LOGGING_CATEGORY(download_info)

namespace downloads {

struct BadMemberName
{
    constexpr const char *what() { return "Bad member name!"; }
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

} // namespace downloads

/**
 * @class DownloadInfo
 * @brief Manages metadata and state for a single file download.
 */
class DownloadInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed from C++")

public:
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
    Q_ENUM(DownloadState)

private:
    Q_PROPERTY(QString downloadID MEMBER m_downloadID NOTIFY downloadIDChanged)
    Q_PROPERTY(QUrl url MEMBER m_url NOTIFY urlChanged)
    Q_PROPERTY(QString hostKey MEMBER m_hostKey NOTIFY hostKeyChanged)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QString path MEMBER m_path NOTIFY pathChanged)
    Q_PROPERTY(QString saveName MEMBER m_saveName NOTIFY saveNameChanged)
    Q_PROPERTY(QString savePath MEMBER m_savePath NOTIFY savePathChanged)
    Q_PROPERTY(qint64 size MEMBER m_size NOTIFY sizeChanged)
    Q_PROPERTY(qint64 lastReceivedByte MEMBER m_lastReceivedByte NOTIFY lastReceivedByteChanged)
    Q_PROPERTY(QString created MEMBER m_created NOTIFY createdChanged)
    Q_PROPERTY(QString modified MEMBER m_modified NOTIFY modifiedChanged)
    Q_PROPERTY(QString accessed MEMBER m_accessed NOTIFY accessedChanged)
    Q_PROPERTY(DownloadState downloadState MEMBER m_downloadState NOTIFY downloadStateChanged)

    // --- Constructors ---
public:
    explicit DownloadInfo(QObject *parent = nullptr);
    DownloadInfo(const QString &downloadID,
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
                 DownloadState downloadState,
                 QObject *parent = nullptr);

    DownloadInfo(const DownloadInfo &downloadInfo);
    DownloadInfo &operator=(const DownloadInfo &downloadInfo);

    DownloadInfo(DownloadInfo &&downloadInfo);
    DownloadInfo &operator=(DownloadInfo &&downloadInfo);

    ~DownloadInfo();

    // --- Set Methods ---
public:
    void setLastReceivedByte(qint64 bytesCount)
    {
        m_lastReceivedByte = bytesCount;
        emit lastReceivedByteChanged();
    }
    void setDownloadState(DownloadState state)
    {
        m_downloadState = state;
        emit downloadStateChanged();
    }

    // --- Signals ---
signals:
    void downloadStateChanged();
    void downloadIDChanged();
    void urlChanged();
    void hostKeyChanged();
    void nameChanged();
    void pathChanged();
    void saveNameChanged();
    void savePathChanged();
    void sizeChanged();
    void lastReceivedByteChanged();
    void createdChanged();
    void modifiedChanged();
    void accessedChanged();

    // --- Members ---
public:
    QString m_downloadID;           ///< Download ID
    QUrl m_url;                     ///< Host URL
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
    DownloadState m_downloadState;  ///< Download state

    bool m_forDelete{false};
};

#endif // DOWNLOADINFO_HPP
