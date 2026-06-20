#ifndef WEBDAVFILE_HPP
#define WEBDAVFILE_HPP

#include <QDateTime>
#include <QLoggingCategory>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(client_webdav_item)

class WebdavItem
{
public:
    WebdavItem();

    WebdavItem(const QString &path,
               const QString &name,
               const QString &ext,
               bool dirOrFile,
               const QDateTime &lastModified,
               quint64 size,
               const QString &displayName,
               const QDateTime &createdAt);

    bool isDir() const;
    QString path() const;
    QString name() const;
    QString ext() const;
    QDateTime lastModified() const;
    quint64 size() const;

    QString displayName() const;
    QDateTime createdAt() const;

protected:
    bool m_dirOrFile;
    QString m_path;
    QString m_name;
    QString m_ext;
    QDateTime m_lastModified;
    quint64 m_size;
    QString m_displayName;
    QDateTime m_createdAt;
};

#endif // WEBDAVFILE_HPP
