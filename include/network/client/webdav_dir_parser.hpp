#ifndef QWEBDAVDIRPARSER_HPP
#define QWEBDAVDIRPARSER_HPP

#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QRecursiveMutex>

#include "client_webdav.hpp"
#include "webdav_item.hpp"

Q_DECLARE_LOGGING_CATEGORY(client_webdav_dir_parser)

class WebdavDirParser : public QObject
{
    Q_OBJECT

public:
    explicit WebdavDirParser(QObject *parent = nullptr);
    ~WebdavDirParser();

    //! get all items of a collection
    bool listDirectory(ClientWebDAV *pWebdav, const QString &path, bool recursive = false);
    //! get only information about the collection
    bool getDirectoryInfo(ClientWebDAV *pWebdav, const QString &path);
    //! get only information about a file
    bool getFileInfo(ClientWebDAV *pWebdav, const QString &path);
    bool listItem(ClientWebDAV *pWebdav, const QString &path);

    QList<WebdavItem> getList();
    bool isBusy() const;
    bool isFinished() const;
    QString path() const;

signals:
    void finished();
    void errorChanged(QString);

public slots:
    void abort();

protected slots:
    //void error(QNetworkReply::NetworkError code);
    void replyFinished();
    void replyDeleteLater(QNetworkReply *reply);

protected:
    void parseMultiResponse(const QByteArray &data);
    void parseResponse(const QDomElement &dom);
    void davParsePropstats(const QString &path, const QDomNodeList &propstats);
    int codeFromResponse(const QStringView response);
    QDateTime parseDateTime(const QString &input, const QString &type);

private:
    //    QScopedPointer<QMutex> m_mutex;
    QScopedPointer<QRecursiveMutex> m_mutex;
    ClientWebDAV *m_webdav;
    QNetworkReply *m_reply;
    QList<WebdavItem> m_dirList;
    QString m_path;
    bool m_includeRequestedURI;
    bool m_busy;
    bool m_abort;
};

#endif // QWEBDAVDIRPARSER_HPP
