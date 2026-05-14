#include "file_info.hpp"

Q_LOGGING_CATEGORY(directory_file_info, "directory.file.info")

FileInfo::FileInfo(QObject *parent)
    : QObject(parent)
{
    qCDebug(directory_file_info) << "FileInfo - created";
}

FileInfo::FileInfo(const QString &name,
                   const QString &path,
                   const QString &created,
                   const QString &modified,
                   const QString &accessed,
                   qint64 size,
                   bool isDir,
                   bool isReadable,
                   QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_path(path)
    , m_created(created)
    , m_modified(modified)
    , m_accessed(accessed)
    , m_size(size)
    , m_isDir(isDir)
    , m_isReadable(isReadable)
{
    qCDebug(directory_file_info) << "FileInfo:" << m_name << "- created";
}

FileInfo::FileInfo(const FileInfo &directoryInfo)
{
    *this = directoryInfo;
}

FileInfo &FileInfo::operator=(const FileInfo &directoryInfo)
{
    m_name = directoryInfo.m_name;
    m_path = directoryInfo.m_path;
    m_created = directoryInfo.m_created;
    m_modified = directoryInfo.m_modified;
    m_accessed = directoryInfo.m_accessed;
    m_size = directoryInfo.m_size;
    m_isDir = directoryInfo.m_isDir;
    m_isReadable = directoryInfo.m_isReadable;
    this->setParent(directoryInfo.parent());

    qCDebug(directory_file_info) << "FileInfo:" << m_name << "- copyed";

    return *this;
}

FileInfo::FileInfo(FileInfo &&directoryInfo)
{
    *this = std::move(directoryInfo);
}

FileInfo &FileInfo::operator=(FileInfo &&directoryInfo)
{
    m_name = std::move(directoryInfo.m_name);
    m_path = std::move(directoryInfo.m_path);
    m_created = std::move(directoryInfo.m_created);
    m_modified = std::move(directoryInfo.m_modified);
    m_accessed = std::move(directoryInfo.m_accessed);
    m_size = directoryInfo.m_size;
    m_isDir = directoryInfo.m_isDir;
    m_isReadable = directoryInfo.m_isReadable;
    this->setParent(directoryInfo.parent());

    qCDebug(directory_file_info) << "FileInfo:" << m_name << "- moved";

    return *this;
}

FileInfo::~FileInfo()
{
    qCDebug(directory_file_info) << "FileInfo:" << m_name << "- destroyed";
}
