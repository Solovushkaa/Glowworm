#ifndef CLIENTWEBDAVPROTOCOL_HPP
#define CLIENTWEBDAVPROTOCOL_HPP

#include <QDateTime>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>

Q_DECLARE_LOGGING_CATEGORY(client_webdav_protocol)

/**
 * @brief Main class used to handle the webdav protocol
 */
class ClientWebDAVProtocol : public QNetworkAccessManager
{
    Q_OBJECT

public:
    enum WebdavConnectionType { HTTP = 1, HTTPS };
    enum WebdavLockScope { LOCK_SCOPE_EXCLUSIVE, LOCK_SCOPE_SHARED };
    enum WebdavLockDepth { LOCK_DEPTH_ZERO, LOCK_DEPTH_INFINITY };

    explicit ClientWebDAVProtocol(QObject *parent = nullptr);
    ~ClientWebDAVProtocol();

    typedef QMap<QString, QMap<QString, QVariant> > PropValues;
    typedef QMap<QString, QStringList> PropNames;

    QString hostname() const;
    int port() const;
    QString rootPath() const;
    QString username() const;
    QString password() const;
    WebdavConnectionType connectionType() const;
    bool isSSL() const;

    void setConnectionSettings(const WebdavConnectionType connectionType,
                               const QString &hostname,
                               const QString &rootPath = "/",
                               const QString &username = "",
                               const QString &password = "",
                               int port = 0,
                               const QString &sslCertDigestMd5 = "",
                               const QString &sslCertDigestSha1 = "");

    //! set SSL certificate digests after emitted checkSslCertifcate() signal
    void acceptSslCertificate(const QString &sslCertDigestMd5 = "",
                              const QString &sslCertDigestSha1 = "");

    QNetworkReply *list(const QString &path);
    QNetworkReply *list(const QString &path, int depth);

    QNetworkReply *search(const QString &path, const QString &query);

    QNetworkReply *get(const QString &path);
    QNetworkReply *get(const QString &path, QIODevice *data);
    QNetworkReply *get(const QString &path, QIODevice *data, quint64 fromRangeInBytes);

    QNetworkReply *put(const QString &path, QIODevice *data, const QDateTime &dt = QDateTime());
    QNetworkReply *put(const QString &path,
                       const QByteArray &data,
                       const QDateTime &dt = QDateTime());

    QNetworkReply *mkdir(const QString &dir);
    QNetworkReply *copy(const QString &pathFrom, const QString &pathTo, bool overwrite = false);
    QNetworkReply *move(const QString &pathFrom, const QString &pathTo, bool overwrite = false);
    QNetworkReply *remove(const QString &path);

    QNetworkReply *propfind(const QString &path, const QByteArray &query, int depth = 0);
    QNetworkReply *propfind(const QString &path,
                            const ClientWebDAVProtocol::PropNames &props,
                            int depth = 0);

    QNetworkReply *proppatch(const QString &path, const ClientWebDAVProtocol::PropValues &props);
    QNetworkReply *proppatch(const QString &path, const QByteArray &query);

    /* if secs is < 0 then lock timeout will be Infinite */
    /* if token is not empty then lock will be refreshed */
    QNetworkReply *lock(const QString &path,
                        qint64 secs,
                        const QString &token = QString(),
                        WebdavLockScope scope = LOCK_SCOPE_EXCLUSIVE,
                        WebdavLockDepth depth = LOCK_DEPTH_ZERO,
                        const QString &owner = QString());
    QNetworkReply *unlock(const QString &path, const QString &token);

    //! converts a digest from QByteArray to hexadecimal format ( XX:XX:XX:... with X in [0-9,A-F] )
    static QString digestToHex(const QByteArray &input);
    //! converts a digest from hexadecimal format ( XX:XX:XX:... with X in [0-9,A-F] ) to QByteArray
    static QByteArray hexToDigest(const QString &input);

signals:
    //! signal is emitted when an SSL error occured, the SSL certificates have to be checked
    void checkSslCertifcate(const QList<QSslError> &errors);
    void errorChanged(QString error);

protected slots:
    void replyReadyRead();
    void replyFinished(QNetworkReply *);
    void replyDeleteLater(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);
    void provideAuthenication(QNetworkReply *reply, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

protected:
    QNetworkReply *createCustomRequest(const QString &method,
                                       QNetworkRequest &req,
                                       QIODevice *outgoingData = nullptr);
    QNetworkReply *createCustomRequest(const QString &method,
                                       QNetworkRequest &req,
                                       const QByteArray &outgoingData);

    //! creates the absolute path from m_rootPath and relPath
    QString absolutePath(const QString &relPath);
private:

    QNetworkRequest buildRequest();

private:
    QMap<QNetworkReply *, QIODevice *> m_outDataDevices;
    QMap<QNetworkReply *, QIODevice *> m_inDataDevices;

    QString m_rootPath;
    QString m_username;
    QString m_password;
    QUrl m_baseUrl;

    WebdavConnectionType m_currentConnectionType;

    QNetworkReply *m_authenticator_lastReply;

    // MD5 and SHA1 digests to accept explicitly a SSL certificate
    QByteArray m_sslCertDigestMd5;
    QByteArray m_sslCertDigestSha1;
};

#endif // CLIENTWEBDAVPROTOCOL_HPP
