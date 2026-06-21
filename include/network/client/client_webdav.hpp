#ifndef CLIENTWEBDAV_HPP
#define CLIENTWEBDAV_HPP

#include <QFile>
#include <QLoggingCategory>
#include <QTimer>
#include "connection_info.hpp"
#include "directory_manager.hpp"
#include "download_info.hpp"
#include "webdav_dir_parser.hpp"

Q_DECLARE_LOGGING_CATEGORY(client_webdav)

class ClientWebDAV : public QObject
{
    Q_OBJECT

public:
    ClientWebDAV(ConnectionInfo *connectionInfo,
                 DirectoryManager &directoryManager,
                 ClientWebDAVProtocol::WebdavConnectionType webDavProtocol
                 = ClientWebDAVProtocol::WebdavConnectionType::HTTPS,
                 QObject *parent = nullptr);
    ~ClientWebDAV();

    void connectToServer();
    void disconnectFromServer();

    void sendPing();
    void getDirectory(const QString &dirPath);

    void getFile(DownloadInfo *downloadInfo);

    void setDownloadInfo(DownloadInfo *downloadInfo);

public slots:
    void onConnected();
    void onDisconnected();

    void onPong();

    void onDirectoryReceived();

    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onFileReceived();

    void printError(QString error);

signals:
    void connected(ConnectionInfo *connectionInfo);
    void disconnected(ConnectionInfo *connectionInfo);
    void statusCodeChanged(int statusCode);

    void currentDirectoryChanged();

    void fileReceived(const QString &filePath);

    void errorOccurred(const QString &errorString);

private:
    ConnectionInfo *r_connectionInfo;
    DownloadInfo *r_downloadInfo;

    ClientWebDAVProtocol m_webDavProtocol;
    WebdavDirParser m_webDavParser;

    QString m_path;
    QFile *m_saveFile;

    DirectoryManager &r_directoryManager;

private:
    QTimer m_pingTimer;

    int m_pongTimeout{3000}; // 3 sec
    int m_missedPongs{0};
};

#endif // CLIENTWEBDAV_HPP
