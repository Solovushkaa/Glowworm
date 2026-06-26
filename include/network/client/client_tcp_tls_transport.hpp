#ifndef CLIENTTCPTLSTRANSPORT_HPP
#define CLIENTTCPTLSTRANSPORT_HPP

#include <QFile>
#include <QSslError>
#include <QUrl>
#include "download_info.hpp"
#include "message_socket.hpp"

Q_DECLARE_LOGGING_CATEGORY(client_tcp_tls_transport)

class ClientMessageSocket : public MessageSocket
{
    Q_OBJECT

public:
    explicit ClientMessageSocket(QObject *parent)
        : MessageSocket(parent)
    {}

    DownloadInfo *getDownloadInfo() const { return m_downloadInfo; }
    void setDownloadInfo(DownloadInfo *downloadInfo) { m_downloadInfo = downloadInfo; }

private:
    DownloadInfo *m_downloadInfo;
};

class ClientTcpTlsTransport : public QObject
{
    Q_OBJECT

public:
    ClientTcpTlsTransport(const QString &outputFileName, QObject *parent = nullptr);
    ~ClientTcpTlsTransport();

    void getFile(const QString &address, qint16 port, DownloadInfo *downloadInfo);
    void getFileFromRelay(const QString &address,
                          qint16 port,
                          const QString &userName,
                          DownloadInfo *downloadInfo);

private:
    void connectSignals();

    void startNewDownload(const QString &address, qint16 port);
    void startNewDownload(const QString &address, qint16 port, const QString &username);
    void requestFile(DownloadInfo *downloadInfo, const QString &username, bool isUsername);

signals:
    void connected();
    void disconnected();
    void fileReceived(const QString &filePath);
    void errorOccurred(QString errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QByteArray &message);
    void onSocketError(QAbstractSocket::SocketError error);
    // void onFileReceived(const QString &);
    void onSslError(const QList<QSslError> &errors);
    void onEncrypted();
    void onEnded();

private:
    ClientMessageSocket m_socket;
    QFile m_outputFile;

public:
    QByteArray expectedCert;
};

#endif // CLIENTTCPTLSTRANSPORT_HPP
