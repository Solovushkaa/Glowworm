#include "client_webdav.hpp"
#include <QAuthenticator>
#include <QBuffer>
#include <QFile>

Q_LOGGING_CATEGORY(client_webdav, "client.webdav")

ClientWebDAV::ClientWebDAV(QObject *parent)
    : QNetworkAccessManager(parent)
    , m_rootPath()
    , m_username()
    , m_password()
    , m_baseUrl()
    , m_currentConnectionType(ClientWebDAV::HTTP)
    , m_authenticator_lastReply(nullptr)
    , m_sslCertDigestMd5("")
    , m_sslCertDigestSha1("")

{
    qRegisterMetaType<QNetworkReply *>("QNetworkReply*");
    connect(this, &ClientWebDAV::finished, this, &ClientWebDAV::replyFinished);
    connect(this, &ClientWebDAV::authenticationRequired, this, &ClientWebDAV::provideAuthenication);

    qCDebug(client_webdav) << "ClientWebDAV - created";
}

ClientWebDAV::~ClientWebDAV()
{
    qCDebug(client_webdav) << "ClientWebDAV - destroyed";
}

QString ClientWebDAV::hostname() const
{
    return m_baseUrl.host();
}

int ClientWebDAV::port() const
{
    return m_baseUrl.port();
}

QString ClientWebDAV::rootPath() const
{
    return m_rootPath;
}

QString ClientWebDAV::username() const
{
    return m_username;
}

QString ClientWebDAV::password() const
{
    return m_password;
}

ClientWebDAV::QWebdavConnectionType ClientWebDAV::connectionType() const
{
    return m_currentConnectionType;
}

bool ClientWebDAV::isSSL() const
{
    return (m_currentConnectionType == ClientWebDAV::HTTPS);
}

void ClientWebDAV::setConnectionSettings(const QWebdavConnectionType connectionType,
                                         const QString &hostname,
                                         const QString &rootPath,
                                         const QString &username,
                                         const QString &password,
                                         int port,
                                         const QString &sslCertDigestMd5,
                                         const QString &sslCertDigestSha1)
{
    qCDebug(client_webdav) << "Set connection settings";

    m_rootPath = rootPath;

    if ((m_rootPath.size() > 0) && (m_rootPath.endsWith("/")))
        m_rootPath.chop(1);

    QString uriScheme;
    switch (connectionType) {
    case ClientWebDAV::HTTP:
        uriScheme = "http";
        break;
    case ClientWebDAV::HTTPS:
        uriScheme = "https";
        break;
    }

    m_currentConnectionType = connectionType;

    m_baseUrl.setScheme(uriScheme);
    m_baseUrl.setHost(hostname);
    m_baseUrl.setPath(rootPath);

    if (port != 0) {
        // use user-defined port number
        if (!(((port == 80) && (m_currentConnectionType == ClientWebDAV::HTTP))
              || ((port == 443) && (m_currentConnectionType == ClientWebDAV::HTTPS))))
            m_baseUrl.setPort(port);
    }

    m_sslCertDigestMd5 = hexToDigest(sslCertDigestMd5);
    m_sslCertDigestSha1 = hexToDigest(sslCertDigestSha1);

    m_username = username;
    m_password = password;

    qCInfo(client_webdav) << "Connection settings set";
}

void ClientWebDAV::acceptSslCertificate(const QString &sslCertDigestMd5,
                                        const QString &sslCertDigestSha1)
{
    m_sslCertDigestMd5 = hexToDigest(sslCertDigestMd5);
    m_sslCertDigestSha1 = hexToDigest(sslCertDigestSha1);
}

void ClientWebDAV::replyReadyRead()
{
    auto reply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (reply->bytesAvailable() < 256'000)
        return;

    QIODevice *dataIO = m_inDataDevices.value(reply, 0);
    if (dataIO == nullptr)
        return;
    dataIO->write(reply->readAll());
}

void ClientWebDAV::replyFinished(QNetworkReply *reply)
{
    qCDebug(client_webdav) << "Reply finished";

    disconnect(reply, &QNetworkReply::redirectAllowed, this, &ClientWebDAV::replyReadyRead);
    disconnect(reply, &QNetworkReply::errorOccurred, this, &ClientWebDAV::replyError);

    QIODevice *dataIO = m_inDataDevices.value(reply, 0);
    if (dataIO != 0) {
        dataIO->write(reply->readAll());
        static_cast<QFile *>(dataIO)->flush();
        dataIO->close();
        delete dataIO;
    }
    m_inDataDevices.remove(reply);

    QMetaObject::invokeMethod(this,
                              "replyDeleteLater",
                              Qt::QueuedConnection,
                              Q_ARG(QNetworkReply *, reply));
}

void ClientWebDAV::replyDeleteLater(QNetworkReply *reply)
{
    QIODevice *outDataDevice = m_outDataDevices.value(reply, 0);
    if (outDataDevice != nullptr)
        outDataDevice->deleteLater();
    m_outDataDevices.remove(reply);
}

void ClientWebDAV::replyError(QNetworkReply::NetworkError)
{
    auto reply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (reply == nullptr)
        return;

    qCDebug(client_webdav) << "Reply error. reply->url() == "
                           << reply->url().toString(QUrl::RemoveUserInfo);

    if (reply->error() == QNetworkReply::OperationCanceledError) {
        QIODevice *dataIO = m_inDataDevices.value(reply, 0);
        if (dataIO != nullptr) {
            delete dataIO;
            m_inDataDevices.remove(reply);
        }
        return;
    }

    emit errorChanged(reply->errorString());
}

void ClientWebDAV::provideAuthenication(QNetworkReply *reply, QAuthenticator *authenticator)
{
#ifdef DEBUG
    qCDebug(client_webdav) << "ClientWebDAV::authenticationRequired()";
    QVariantHash opts = authenticator->options();
    QVariant optVar;
    foreach (optVar, opts) {
        qCDebug(client_webdav) << "ClientWebDAV::authenticationRequired()  option == "
                               << optVar.toString();
    }
#endif

    if (reply == m_authenticator_lastReply) {
        reply->abort();
        emit errorChanged("WebDAV server requires authentication. Check WebDAV share settings!");
        reply->deleteLater();
        reply = nullptr;
    }

    m_authenticator_lastReply = reply;

    authenticator->setUser(m_username);
    authenticator->setPassword(m_password);
}

void ClientWebDAV::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    qCDebug(client_webdav) << "SSL error. reply->url == "
                           << reply->url().toString(QUrl::RemoveUserInfo);

    QSslCertificate sslcert = errors[0].certificate();

    if ((sslcert.digest(QCryptographicHash::Md5) == m_sslCertDigestMd5)
        && (sslcert.digest(QCryptographicHash::Sha1) == m_sslCertDigestSha1)) {
        // user accepted this SSL certifcate already ==> ignore SSL errors
        reply->ignoreSslErrors();
    } else {
        // user has to check the SSL certificate and has to accept manually
        emit checkSslCertifcate(errors);
        reply->abort();
    }
}

QString ClientWebDAV::digestToHex(const QByteArray &input)
{
    QByteArray inputHex = input.toHex();

    QString result = "";
    for (int i = 0; i < inputHex.size(); i += 2) {
        result.append(inputHex.at(i));
        result.append(inputHex.at(i + 1));
        result.append(":");
    }
    result.chop(1);
    result = result.toUpper();

    return result;
}

QByteArray ClientWebDAV::hexToDigest(const QString &input)
{
    QByteArray result;
    int i = 2;
    int l = static_cast<int>(input.size());
    result.append(input.left(2).toLatin1());
    while ((i < l) && (input.at(i) == ':')) {
        ++i;
        result.append(input.mid(i, 2).toLatin1());
        i += 2;
    }
    return QByteArray::fromHex(result);
}

QString ClientWebDAV::absolutePath(const QString &relPath)
{
    return QString(m_rootPath + relPath);
}

QNetworkReply *ClientWebDAV::createCustomRequest(const QString &method,
                                                 QNetworkRequest &req,
                                                 QIODevice *outgoingData)
{
    if (outgoingData != nullptr && outgoingData->size() != 0) {
        req.setHeader(QNetworkRequest::ContentLengthHeader, outgoingData->size());
        req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");
    }

#ifdef DEBUG
    qCDebug(client_webdav) << " ClientWebDAV::createRequest1";
    qCDebug(client_webdav) << "   " << method << " " << req.url().toString();
    QList<QByteArray> rawHeaderList = req.rawHeaderList();
    QByteArray rawHeaderItem;
    foreach (rawHeaderItem, rawHeaderList) {
        qqCDebug(client_webdav) << "   " << rawHeaderItem << ": " << req.rawHeader(rawHeaderItem);
    }
#endif

    return sendCustomRequest(req, method.toLatin1(), outgoingData);
}

QNetworkReply *ClientWebDAV::createCustomRequest(const QString &method,
                                                 QNetworkRequest &req,
                                                 const QByteArray &outgoingData)
{
    auto dataIO = new QBuffer;
    dataIO->setData(outgoingData);
    dataIO->open(QIODevice::ReadOnly);

#ifdef DEBUG
    qCDebug(client_webdav) << " ClientWebDAV::createRequest2";
    qCDebug(client_webdav) << "   " << method << " " << req.url().toString();
    QList<QByteArray> rawHeaderList = req.rawHeaderList();
    QByteArray rawHeaderItem;
    foreach (rawHeaderItem, rawHeaderList) {
        qCDebug(client_webdav) << "   " << rawHeaderItem << ": " << req.rawHeader(rawHeaderItem);
    }
#endif

    QNetworkReply *reply = createCustomRequest(method, req, dataIO);
    m_outDataDevices.insert(reply, dataIO);
    return reply;
}

QNetworkReply *ClientWebDAV::list(const QString &path)
{
    qCDebug(client_webdav) << "ClientWebDAV::list() path = " << path;

    return list(path, 1);
}

QNetworkReply *ClientWebDAV::list(const QString &path, int depth)
{
    ClientWebDAV::PropNames query;
    QStringList props;

    // Small set of properties
    // href in response contains also the name
    // e.g. /container/front.html
    props << "getlastmodified"; // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getlastmodified
    // e.g. Mon, 12 Jan 1998 09:25:56 GMT
    props << "getcontentlength"; // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontentlength
    // e.g. "4525"
    props << "resourcetype"; // http://www.webdav.org/specs/rfc4918.html#PROPERTY_resourcetype
    // e.g. "collection" for a directory

    // Following properties are available as well.
    //props << "creationdate";          // http://www.webdav.org/specs/rfc4918.html#PROPERTY_creationdate
    // e.g. "1997-12-01T18:27:21-08:00"
    //props << "displayname";           // http://www.webdav.org/specs/rfc4918.html#PROPERTY_displayname
    // e.g. "Example HTML resource"
    //props << "getcontentlanguage";    // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontentlanguage
    // e.g. "en-US"
    //props << "getcontenttype";        // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontenttype
    // e.g "text/html"
    //props << "getetag";               // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getetag
    // e.g. "zzyzx"

    // Additionally, there are also properties for locking

    query["DAV:"] = props;

    return propfind(path, query, depth);
}

QNetworkReply *ClientWebDAV::search(const QString &path, const QString &q)
{
    QByteArray query = "<?xml version=\"1.0\"?>\r\n";
    query.append("<D:searchrequest xmlns:D=\"DAV:\">\r\n");
    query.append(q.toUtf8());
    query.append("</D:searchrequest>\r\n");

    QNetworkRequest req;

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return this->createCustomRequest("SEARCH", req, query);
}

QNetworkReply *ClientWebDAV::get(const QString &path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    qCDebug(client_webdav) << "ClientWebDAV::get() url = "
                           << req.url().toString(QUrl::RemoveUserInfo);

    req.setUrl(reqUrl);

    return QNetworkAccessManager::get(req);
}

QNetworkReply *ClientWebDAV::get(const QString &path, QIODevice *data)
{
    return get(path, data, 0);
}

QNetworkReply *ClientWebDAV::get(const QString &path, QIODevice *data, quint64 fromRangeInBytes)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    qCDebug(client_webdav) << "ClientWebDAV::get() url = "
                           << req.url().toString(QUrl::RemoveUserInfo);

    if (fromRangeInBytes > 0) {
        // RFC2616 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        QByteArray fromRange = "bytes=" + QByteArray::number(fromRangeInBytes)
                               + "-"; // byte-ranges-specifier
        req.setRawHeader("Range", fromRange);
    }

    QNetworkReply *reply = QNetworkAccessManager::get(req);
    m_inDataDevices.insert(reply, data);
    connect(reply, &QNetworkReply::readyRead, this, &ClientWebDAV::replyReadyRead);
    connect(reply, &QNetworkReply::errorOccurred, this, &ClientWebDAV::replyError);

    return reply;
}

QNetworkReply *ClientWebDAV::put(const QString &path, QIODevice *data, const QDateTime &dt)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    if (dt.isValid()) {
        QLocale us(QLocale::English, QLocale::UnitedStates);
        req.setRawHeader("Date", us.toString(dt.toUTC(), "ddd, dd MMM yyyy hh:mm:ss").toUtf8());
    }

    qCDebug(client_webdav) << "ClientWebDAV::put() url = "
                           << req.url().toString(QUrl::RemoveUserInfo)
                           << " date = " << req.rawHeader("Date");

    return QNetworkAccessManager::put(req, data);
}

QNetworkReply *ClientWebDAV::put(const QString &path, const QByteArray &data, const QDateTime &dt)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    if (dt.isValid()) {
        QLocale us(QLocale::English, QLocale::UnitedStates);
        req.setRawHeader("Date", us.toString(dt.toUTC(), "ddd, dd MMM yyyy hh:mm:ss").toUtf8());
    }

    qCDebug(client_webdav) << "ClientWebDAV::put() url = "
                           << req.url().toString(QUrl::RemoveUserInfo)
                           << " date = " << req.rawHeader("Date");

    return QNetworkAccessManager::put(req, data);
}

QNetworkReply *ClientWebDAV::propfind(const QString &path,
                                      const ClientWebDAV::PropNames &props,
                                      int depth)
{
    QString query;

    query = R"(<?xml version="1.0" encoding="utf-8" ?>)";
    query += R"(<D:propfind xmlns:D="DAV:" >)";
    query += "<D:prop>";
    for (const auto &[ns, propMap] : props.asKeyValueRange()) {
        for (const auto &key : std::as_const(propMap)) {
            if (ns == "DAV:")
                query += "<D:" + key + "/>";
            else
                query += "<" + key + " xmlns=\"" + ns + "\"/>";
        }
    }
    query += "</D:prop>";
    query += "</D:propfind>";
    return propfind(path, query.toUtf8(), depth);
}

QNetworkReply *ClientWebDAV::propfind(const QString &path, const QByteArray &query, int depth)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);
    req.setRawHeader("Depth",
                     depth == 2 ? QString("infinity").toUtf8() : QString::number(depth).toUtf8());

    return createCustomRequest("PROPFIND", req, query);
}

QNetworkReply *ClientWebDAV::proppatch(const QString &path, const ClientWebDAV::PropValues &props)
{
    QString query;

    query = R"(<?xml version="1.0" encoding="utf-8" ?>)";
    query += R"(<D:proppatch xmlns:D="DAV:" >)";
    query += "<D:prop>";
    for (const auto &[ns, _] : props.asKeyValueRange()) {
        QMap<QString, QVariant>::const_iterator i;

        for (i = props[ns].constBegin(); i != props[ns].constEnd(); ++i) {
            if (ns == "DAV:") {
                query += "<D:" + i.key() + ">";
                query += i.value().toString();
                query += "</D:" + i.key() + ">";
            } else {
                query += "<" + i.key() + " xmlns=\"" + ns + "\">";
                query += i.value().toString();
                query += "</" + i.key() + " xmlns=\"" + ns + "\"/>";
            }
        }
    }
    query += "</D:prop>";
    query += "</D:propfind>";

    return proppatch(path, query.toUtf8());
}

QNetworkReply *ClientWebDAV::proppatch(const QString &path, const QByteArray &query)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createCustomRequest("PROPPATCH", req, query);
}

QNetworkReply *ClientWebDAV::lock(const QString &path,
                                  qint64 secs,
                                  const QString &token,
                                  QWebdavLockScope scope,
                                  QWebdavLockDepth depth,
                                  const QString &owner)
{
    QString lockscope = scope == LOCK_SCOPE_EXCLUSIVE ? "exclusive" : "shared";

    QByteArray query = "<?xml version=\"1.0\"?>\r\n";
    query.append("<D:lockinfo xmlns:D=\"DAV:\">\r\n");
    query.append(" <D:lockscope><D:" + lockscope.toUtf8() + "/>" + "</D:lockscope>\r\n");
    query.append(" <D:locktype><D:write/></D:locktype>\r\n");
    if (!owner.isEmpty())
        query.append(" <D:owner><D:href>" + owner.toUtf8() + "</D:href></D:owner>\r\n");
    query.append("</D:lockinfo>\r\n");

    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    QString lockdepth = depth == LOCK_DEPTH_INFINITY ? "infinity" : "0";
    req.setRawHeader("Depth", lockdepth.toUtf8());

    QString seconds = secs < 0 ? "Infinite" : "Second-" + QString::number(secs);
    req.setRawHeader("Timeout", seconds.toUtf8());

    if (!token.isEmpty()) {
        req.setRawHeader("If", "(<" + token.toUtf8() + ">)");
    }

    return createCustomRequest("LOCK", req, query);
}

QNetworkReply *ClientWebDAV::unlock(const QString &path, const QString &token)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    req.setRawHeader("Lock-Token", "<" + token.toUtf8() + ">");

    return createCustomRequest("UNLOCK", req);
}

QNetworkReply *ClientWebDAV::mkdir(const QString &path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createCustomRequest("MKCOL", req);
}

QNetworkReply *ClientWebDAV::copy(const QString &pathFrom, const QString &pathTo, bool overwrite)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(pathFrom));

    req.setUrl(reqUrl);

    // RFC4918 Section 10.3 requires an absolute URI for destination raw header
    //  http://tools.ietf.org/html/rfc4918#section-10.3
    // RFC3986 Section 4.3 specifies the term absolute URI
    //  http://tools.ietf.org/html/rfc3986#section-4.3
    QUrl dstUrl(m_baseUrl);
    //dstUrl.setUserInfo("");
    dstUrl.setPath(absolutePath(pathTo));
    req.setRawHeader("Destination", dstUrl.toString().toUtf8());

    req.setRawHeader("Depth", "infinity");
    req.setRawHeader("Overwrite", overwrite ? "T" : "F");

    return createCustomRequest("COPY", req);
}

QNetworkReply *ClientWebDAV::move(const QString &pathFrom, const QString &pathTo, bool overwrite)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(pathFrom));

    req.setUrl(reqUrl);

    // RFC4918 Section 10.3 requires an absolute URI for destination raw header
    //  http://tools.ietf.org/html/rfc4918#section-10.3
    // RFC3986 Section 4.3 specifies the term absolute URI
    //  http://tools.ietf.org/html/rfc3986#section-4.3
    QUrl dstUrl(m_baseUrl);
    //dstUrl.setUserInfo("");
    dstUrl.setPath(absolutePath(pathTo));
    req.setRawHeader("Destination", dstUrl.toString().toUtf8());

    req.setRawHeader("Depth", "infinity");
    req.setRawHeader("Overwrite", overwrite ? "T" : "F");

    return createCustomRequest("MOVE", req);
}

QNetworkReply *ClientWebDAV::remove(const QString &path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createCustomRequest("DELETE", req);
}

QNetworkRequest ClientWebDAV::buildRequest()
{
    QNetworkRequest req;
    if (isSSL()) {
        QString concatenated = m_username + ":" + m_password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        req.setRawHeader("Authorization", headerData.toLocal8Bit());
    }
    return req;
}
