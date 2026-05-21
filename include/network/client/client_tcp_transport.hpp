#ifndef CLIENTTCPTRANSPORT_HPP
#define CLIENTTCPTRANSPORT_HPP

#include <QFile>
#include <QUrl>
#include "download_info.hpp"
#include "message_socket.hpp"

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

class ClientTcpTransport : public QObject
{
    Q_OBJECT

public:
    explicit ClientTcpTransport(QObject *parent = nullptr);
    ~ClientTcpTransport();

    void getFile(const QUrl &url, DownloadInfo *downloadInfo);

private:
    void connectSignals(ClientMessageSocket *messenger);

    void startNewDownload(const QUrl &url, const QString &downloadID);
    void requestFile(DownloadInfo *downloadInfo);

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

private:
    QHash<QString, ClientMessageSocket *> m_sockets; // downloadID, socket
    QHash<QString, QFile *> m_outputFiles;           // downloadID, file
};

#endif // CLIENTTCPTRANSPORT_HPP
