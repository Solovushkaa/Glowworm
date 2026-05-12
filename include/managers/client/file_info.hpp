#ifndef FILEINFO_HPP
#define FILEINFO_HPP

#include <QLoggingCategory>
#include <QObject>
#include <QProperty>
#include <QStringView>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

Q_DECLARE_LOGGING_CATEGORY(directory_file_info)

/**
 * @class DirectoryInfo
 * @brief -
 */
class FileInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed from C++")

private:
    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString path MEMBER m_path CONSTANT)
    Q_PROPERTY(QString created MEMBER m_created CONSTANT)
    Q_PROPERTY(QString modified MEMBER m_modified CONSTANT)
    Q_PROPERTY(QString accessed MEMBER m_accessed CONSTANT)
    Q_PROPERTY(qint64 size MEMBER m_size CONSTANT)
    Q_PROPERTY(bool isDir MEMBER m_isDir CONSTANT)
    Q_PROPERTY(bool isReadable MEMBER m_isReadable CONSTANT)

    // --- Constructors ---
public:
    explicit FileInfo(QObject *parent = nullptr);
    FileInfo(const QString &name,
             const QString &path,
             const QString &created,
             const QString &modified,
             const QString &accessed,
             qint64 size,
             bool isDir,
             bool isReadable,
             QObject *parent = nullptr);

    FileInfo(const FileInfo &directoryInfo);
    FileInfo &operator=(const FileInfo &directoryInfo);

    FileInfo(FileInfo &&directoryInfo);
    FileInfo &operator=(FileInfo &&directoryInfo);

    ~FileInfo();

public:
    QString m_name;
    QString m_path;
    QString m_created;
    QString m_modified;
    QString m_accessed;
    qint64 m_size;
    bool m_isDir;
    bool m_isReadable;
};

#endif // FILEINFO_HPP
