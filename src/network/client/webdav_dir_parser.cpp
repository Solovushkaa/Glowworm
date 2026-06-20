#include "webdav_dir_parser.hpp"

Q_LOGGING_CATEGORY(client_webdav_dir_parser, "client.webdav.dir.parser")

WebdavDirParser::WebdavDirParser(QObject *parent)
    : QObject(parent)
    , m_webdav(0)
    , m_reply(0)
    , m_path()
    , m_includeRequestedURI(false)
    , m_busy(false)
    , m_abort(false)
{
//    m_mutex.reset(new QMutex(QMutex::Recursive));
    m_mutex.reset(new QRecursiveMutex());

    qCDebug(client_webdav_dir_parser) << "WebdavDirParser - created";
}

WebdavDirParser::~WebdavDirParser()
{
    if (m_reply!=0) {
        m_reply->deleteLater();
        m_reply = 0;
    }

    qCDebug(client_webdav_dir_parser) << "WebdavDirParser - destroyed";
}

bool WebdavDirParser::listDirectory(ClientWebDAVProtocol *pWebdav, const QString &path, bool recursive)
{
    if (m_busy)
        return false;

    if (m_reply!=0)
        return false;

    if (pWebdav==0)
        return false;

    if (path.isEmpty())
        return false;

    if (!path.endsWith("/"))
        return false;

    m_webdav = pWebdav;
    m_path = path;
    m_busy = true;
    m_abort = false;
    m_includeRequestedURI = false;

    m_reply = pWebdav->list(path, recursive ? 2 : 1);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));

    if (!m_dirList.isEmpty())
        m_dirList.clear();

    return true;
}

bool WebdavDirParser::listItem(ClientWebDAVProtocol *pWebdav, const QString &path)
{
    if (m_busy)
        return false;

    if (m_reply!=0)
        return false;

    if (pWebdav==0)
        return false;

    if (path.isEmpty())
        return false;

    m_webdav = pWebdav;
    m_path = path;
    m_busy = true;
    m_includeRequestedURI = true;

    m_reply = pWebdav->list(path, 0);

    if (!m_dirList.isEmpty())
        m_dirList.clear();

    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));

    return true;
}

bool WebdavDirParser::getDirectoryInfo(ClientWebDAVProtocol *pWebdav, const QString &path)
{
    if (!path.endsWith("/"))
        return false;

    return listItem(pWebdav, path);
}

bool WebdavDirParser::getFileInfo(ClientWebDAVProtocol *pWebdav, const QString &path)
{
    if (path.endsWith("/"))
        return false;

    return listItem(pWebdav, path);
}

QList<WebdavItem> WebdavDirParser::getList()
{
    return m_dirList;
}

bool WebdavDirParser::isBusy() const
{
    return m_busy;
}

bool WebdavDirParser::isFinished() const
{
    if (m_reply!=0)
        return m_reply->isFinished();
    else
        return true;
}

QString WebdavDirParser::path() const
{
    return m_path;
}

void WebdavDirParser::abort()
{
    m_abort = true;

    if (m_reply!=0)
        m_reply->abort();

    m_reply = 0;
    m_busy = false;
}

void WebdavDirParser::replyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());

    if (m_reply != reply) {
        QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
        return;
    }

    if (m_abort)
        return;

    {
        QMutexLocker locker(m_mutex.data());

        QString contentType = m_reply->header(QNetworkRequest::ContentTypeHeader).toString();

        qCDebug(client_webdav_dir_parser) << "   Reply finished. Content header:" << contentType;

        if ( (m_reply->error() != QNetworkReply::NoError) && (m_reply->error() != QNetworkReply::OperationCanceledError) ) {
            QString errStr = m_reply->errorString();
            errStr = errStr.right(errStr.size()-errStr.indexOf("server replied:")+1);
            emit errorChanged(errStr);

            qCDebug(client_webdav_dir_parser)
                << "   Reply has error. Error:" << m_reply->errorString()
                << "Code:" << m_reply->error();
        }
        else {
            QByteArray data = m_reply->readAll();

            if (contentType.contains("xml")) {
                parseMultiResponse(data);
            }
        }

        m_reply = 0;
    }

    QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
}

void WebdavDirParser::replyDeleteLater(QNetworkReply *reply)
{
    if (reply==0)
        return;

    qCDebug(client_webdav_dir_parser) << "WebdavDirParser::replyDeleteLater()   reply->url == "
                                      << reply->url().toString(QUrl::RemoveUserInfo);
    qCDebug(client_webdav_dir_parser)
        << "WebdavDirParser::replyDeleteLater()      reply->isFinished() == "
        << reply->isFinished();
    qCDebug(client_webdav_dir_parser)
        << "WebdavDirParser::replyDeleteLater()      reply->bytesAvailable() == "
        << reply->bytesAvailable();
    qCDebug(client_webdav_dir_parser)
        << "WebdavDirParser::replyDeleteLater()      reply->bytesToWrite() == "
        << reply->bytesToWrite();

    if ((!reply->isFinished()) || reply->bytesToWrite() || reply->bytesAvailable()) {
        qCDebug(client_webdav_dir_parser)
            << "WebdavDirParser::replyDeleteLater()      reinvoke replyDeleteLater()";

        reply->readAll();
        reply->close();
        QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
        return;
    }

    disconnect(reply, 0, 0, 0);
    reply->deleteLater();
    m_busy = false;

    emit finished();

    qCDebug(client_webdav_dir_parser)
        << "WebdavDirParser::replyDeleteLater()      reply->deleteLater() done";
}

void WebdavDirParser::parseMultiResponse(const QByteArray &data)
{
    if (m_abort)
        return;

    QDomDocument multiResponse;
    multiResponse.setContent(data, QDomDocument::ParseOption::UseNamespaceProcessing);

    for(QDomNode n = multiResponse.documentElement().firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if (m_abort)
            return;

        QDomElement thisResponse = n.toElement();

        if(thisResponse.isNull())
            continue;

        QString responseName = QUrl::fromPercentEncoding(thisResponse.namedItem("href").toElement().text().toUtf8());
        if(responseName.isEmpty())
            continue;

        // ingore the path itself within the listing of the path

        // Apache returns only path without scheme and authority
        if ((!m_includeRequestedURI) && ((m_webdav->rootPath()+m_path) == responseName))
            continue;

        // MS IIS returns URL
        if ((!m_includeRequestedURI) && responseName.startsWith("http")) {
            QUrl checkUrl(responseName);
            if ((m_webdav->rootPath()+m_path) == checkUrl.path())
                continue;
        }

        if (m_abort)
            return;

        parseResponse(thisResponse);

        // container without slash at the end is a wrong answer
        // remove this item from the list
        if ((!m_includeRequestedURI) && m_dirList.last().isDir() && !responseName.endsWith("/")) {
            if (responseName.startsWith("http")) {
                // box.com
                QUrl checkUrl(responseName);
                if ((m_webdav->rootPath()+m_path) == checkUrl.path()+"/")
                    m_dirList.removeLast();
            } else {
                // dav-pocket.appspot.com
                if ((m_webdav->rootPath()+m_path) == (responseName+"/"))
                    m_dirList.removeLast();
            }
        }
    }
}

void WebdavDirParser::parseResponse(const QDomElement &dom)
{
    if (m_abort)
        return;

    QDomElement href = dom.namedItem("href").toElement();
    if (href.isNull()) return;

    QString urlStr = QUrl::fromPercentEncoding(href.text().toUtf8());
    QDomNodeList propstats = dom.elementsByTagName("propstat");
    davParsePropstats(urlStr, propstats);
}

void WebdavDirParser::davParsePropstats(const QString &path, const QDomNodeList &propstats)
{
    if (m_abort)
        return;


    QString path_;
    QString name;
    QString ext;
    bool dirOrFile = false;
    QDateTime lastModified;
    quint64 size = 0;

    if (path.startsWith("http")) { // with scheme and authority
        QUrl pathUrl(path);
        path_ = pathUrl.path();
    } else
        path_ = path; // without scheme and authority

    QString displayName;
    QDateTime createdAt;

    // name
    // QStringList pathElements = path_.split('/', QString::SkipEmptyParts);
    QStringList pathElements = path_.split('/', Qt::SkipEmptyParts);
    name = pathElements.isEmpty() ? "/" : pathElements.back();

    for (int i = 0; i < propstats.count(); ++i) {
        QDomElement propstat = propstats.item(i).toElement();
        QDomElement status = propstat.namedItem( "status" ).toElement();

        if ( status.isNull() ) {
            qCDebug(client_webdav_dir_parser) << "Error, no status code in this propstat";
            return;
        }

        if (m_abort)
            return;

        int code = codeFromResponse( status.text() );

        if (code == 404) // property not available
            continue;

        QDomElement prop = propstat.namedItem( "prop" ).toElement();

        if ( prop.isNull() ) {
            qCDebug(client_webdav_dir_parser) << "Error: no prop segment in this propstat.";
            return;
        }

        for ( QDomNode n = prop.firstChild(); !n.isNull(); n = n.nextSibling() ) {
            QDomElement property = n.toElement();

            if (m_abort)
                return;

            if (property.isNull())
                continue;

            if ( property.namespaceURI() != "DAV:" ) {
                // parse only DAV namespace properties
                continue;
            }

            if ( property.tagName() == "getcontentlength" )
                size = property.text().toULongLong();
            else if ( property.tagName() == "getlastmodified" )
                lastModified = parseDateTime( property.text(), property.attribute("dt") );
            else if ( property.tagName() == "resourcetype" )
            {
                if ( !property.namedItem( "collection" ).toElement().isNull() )
                    dirOrFile = true;
            } else if (property.tagName() == "creationdate")
                createdAt = parseDateTime(property.text(), property.attribute("dt"));
            else if (property.tagName() == "displayname")
                displayName = property.text();

            else
                qCDebug(client_webdav_dir_parser)
                    << "Found unknown WEBDAV property: " << property.tagName() << property.text();
        }
    }

    // check directory path
    if (dirOrFile && !path_.endsWith("/"))
        path_.append("/");

    // get file extension
    if (!dirOrFile) {
        int fileTypeSepIdx = name.lastIndexOf(".");
        if (fileTypeSepIdx == -1) {
            ext = "";
        } else {
            ext = name.right(name.size() - fileTypeSepIdx - 1).toUpper();
        }
    }

    path_.remove(0,m_webdav->rootPath().size());

    m_dirList.append(
        WebdavItem(path_, name, ext, dirOrFile, lastModified, size, displayName, createdAt));
}

int WebdavDirParser::codeFromResponse(const QStringView response)
{
    int firstSpace = response.indexOf(' ');
    int secondSpace = response.indexOf(' ', firstSpace + 1);
    return response.sliced(firstSpace + 1, secondSpace - firstSpace - 1).toInt();
}

QDateTime WebdavDirParser::parseDateTime(const QString &input, const QString &type)
{
    QDateTime datetime;
    QLocale usLocal(QLocale::English, QLocale::UnitedStates);

    if ( type == "dateTime.tz" )
        datetime =  QDateTime::fromString(input, Qt::ISODate );
    else if ( type == "dateTime.rfc1123" )
        datetime = usLocal.toDateTime( input );

    if (datetime.isValid())
        return datetime;

    datetime = usLocal.toDateTime(input.left(25), "ddd, dd MMM yyyy hh:mm:ss");
    if (datetime.isValid())
        return datetime;
    datetime = usLocal.toDateTime(input.left(19), "yyyy-MM-ddThh:mm:ss");
    if (datetime.isValid())
        return datetime;
    datetime = usLocal.toDateTime(input.mid(5, 20) , "d MMM yyyy hh:mm:ss");
    if (datetime.isValid())
        return datetime;
    QDate date;
    QTime time;

    date = usLocal.toDate(input.mid(5, 11) , "d MMM yyyy");
    time = usLocal.toTime(input.mid(17, 8) , "hh:mm:ss");
    datetime = QDateTime(date, time);

#ifdef DEBUG
    if(!datetime.isValid())
        qCDebug(client_webdav_dir_parser)
            << "WebdavDirParser::parseDateTime() | Unknown date time format:" << input;
#endif

    return datetime;
}
