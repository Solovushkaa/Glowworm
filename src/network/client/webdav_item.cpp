#include "webdav_item.hpp"

Q_LOGGING_CATEGORY(client_webdav_item, "client.webdav.item")

WebdavItem::WebdavItem()
    : m_dirOrFile()
    , m_path()
    , m_name()
    , m_ext()
    , m_lastModified()
    , m_size(0)
    , m_displayName()
    , m_createdAt()
{}

WebdavItem::WebdavItem(const QString &path,
                       const QString &name,
                       const QString &ext,
                       bool dirOrFile,
                       const QDateTime &lastModified,
                       quint64 size,
                       const QString &displayName,
                       const QDateTime &createdAt)
    : m_dirOrFile(dirOrFile)
    , m_path(path)
    , m_name(name)
    , m_ext(ext)
    , m_lastModified(lastModified)
    , m_size(size)
    , m_displayName(displayName)
    , m_createdAt(createdAt)
{}

bool WebdavItem::isDir() const
{
    return m_dirOrFile;
}

QString WebdavItem::path() const
{
    return m_path;
}

QString WebdavItem::name() const
{
    return m_name;
}

QString WebdavItem::ext() const
{
    return m_ext;
}

QDateTime WebdavItem::lastModified() const
{
    return m_lastModified;
}

quint64 WebdavItem::size() const
{
    return m_size;
}

QString WebdavItem::displayName() const
{
    return m_displayName;
}

QDateTime WebdavItem::createdAt() const
{
    return m_createdAt;
}
