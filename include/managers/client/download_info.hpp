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
#include <QProperty>
#include <QString>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

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
    Q_PROPERTY(
        QString downloadID MEMBER m_downloadID READ downloadID NOTIFY downloadIDChanged CONSTANT)
    Q_PROPERTY(QUrl url MEMBER m_url READ url NOTIFY urlChanged CONSTANT)
    Q_PROPERTY(QString hostKey MEMBER m_hostKey READ hostKey NOTIFY hostKeyChanged CONSTANT)
    Q_PROPERTY(QString name MEMBER m_name READ name NOTIFY nameChanged CONSTANT)
    Q_PROPERTY(QString path MEMBER m_path READ path NOTIFY pathChanged CONSTANT)
    Q_PROPERTY(QString saveName MEMBER m_saveName READ saveName NOTIFY saveNameChanged CONSTANT)
    Q_PROPERTY(QString savePath MEMBER m_savePath READ savePath NOTIFY savePathChanged CONSTANT)
    Q_PROPERTY(qint64 size MEMBER m_size READ size NOTIFY sizeChanged CONSTANT)
    Q_PROPERTY(qint64 lastReceivedByte MEMBER m_lastReceivedByte READ lastReceivedByte NOTIFY
                   lastReceivedByteChanged CONSTANT)
    Q_PROPERTY(QString created MEMBER m_created READ created NOTIFY createdChanged CONSTANT)
    Q_PROPERTY(QString modified MEMBER m_modified READ modified NOTIFY modifiedChanged CONSTANT)
    Q_PROPERTY(QString accessed MEMBER m_accessed READ accessed NOTIFY accessedChanged CONSTANT)

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
    Q_PROPERTY(DownloadState downloadState READ downloadState WRITE setDownloadState NOTIFY
                   downloadStateChanged BINDABLE bindableDownloadState)

    QBindable<DownloadState> bindableDownloadState() { return &m_downloadState; }

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
                 DownloadState downloadState);

    DownloadInfo(const DownloadInfo &downloadInfo) { *this = downloadInfo; }
    DownloadInfo &operator=(const DownloadInfo &downloadInfo);

    DownloadInfo(DownloadInfo &&downloadInfo) { *this = std::move(downloadInfo); }
    DownloadInfo &operator=(DownloadInfo &&downloadInfo);

    // --- Methods ---
public:
    QString downloadID() const { return m_downloadID; }
    QUrl url() const { return m_url; }
    QString hostKey() const { return m_hostKey; }
    QString name() const { return m_name; }
    QString path() const { return m_path; }
    QString saveName() const { return m_saveName; }
    QString savePath() const { return m_savePath; }
    qint64 size() const { return m_size; }
    qint64 lastReceivedByte() const { return m_lastReceivedByte; }
    QString created() const { return m_created; }
    QString modified() const { return m_modified; }
    QString accessed() const { return m_accessed; }
    DownloadState downloadState() const { return m_downloadState; }

    // --- Slots ---
public slots:
    void setDownloadState(DownloadState state) { m_downloadState = state; }

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
    // QProperty<DownloadState> m_downloadState; ///< Download status

    Q_OBJECT_BINDABLE_PROPERTY(DownloadInfo,
                               DownloadInfo::DownloadState,
                               m_downloadState,
                               &DownloadInfo::downloadStateChanged)
};

#endif // DOWNLOADINFO_HPP
