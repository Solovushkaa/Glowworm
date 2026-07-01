#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QStandardPaths>
#include "client_connection_manager.hpp"
#include "client_tcp_tls_transport.hpp"
#include "client_webdav.hpp"
#include "client_websocket_messenger.hpp"
#include "directory_manager.hpp"
#include "download_manager.hpp"

Q_DECLARE_LOGGING_CATEGORY(client)

/**
 * @class Client
 * @brief The client class
 * 
 * A class for combining the entire client part of the application.
 * The class sends requests to the server and receives responses,
 * as well as interacts with the GUI
 * 
 */
class Client : public QObject
{
    Q_OBJECT

    // --- Public class methods ---
public:
    /**
     * @brief Basic constructor.
     */
    explicit Client(const QString &connectionsFilePath = QStandardPaths::writableLocation(
                                                             QStandardPaths::AppDataLocation)
                                                         + "/appdata/client/savedconnections.json",
                    const QString &downloadsFilePath = QStandardPaths::writableLocation(
                                                           QStandardPaths::AppDataLocation)
                                                       + "/appdata/client/unfinisheddownloads.json",
                    QObject *parent = nullptr);
    ~Client();

    /**
     * @brief changeConnection
     */
    Q_INVOKABLE void setActiveConnection(int index);
    /**
     * @brief Checking the connection to the server.
     */
    Q_INVOKABLE void connectToServer();
    Q_INVOKABLE void disconnectFromServer();
    /**
     * @brief Getting a list of directories.
     */
    Q_INVOKABLE void getNetworkDirectory(const QString &dirPath);

    Q_INVOKABLE void getSystemDirectory(const QString &dirPath);
    /**
     * @brief Getting a file from the server.
     */
    Q_INVOKABLE void getFile(int fileIndex, const QString &saveName, const QString &savePath);
    /**
     * @brief Getting a file from the server.
     */
    Q_INVOKABLE void getFileRange(int fileIndex,
                                  const QString &saveName,
                                  const QString &savePath,
                                  qint64 begin);
    // /**
    //  * @brief Start current download
    //  * @param downloadID Download ID
    //  */
    // Q_INVOKABLE void startDownload(int downloadIndex);
    // /**
    //  * @brief Stop current download
    //  * @param downloadID Download ID
    //  */
    // Q_INVOKABLE void stopDownload(int downloadIndex);

    // Q_INVOKABLE void connectToRelayServer();
    // Q_INVOKABLE void getFileFromRelayServer(int fileIndex,
    //                                         const QString &userName,
    //                                         const QString &saveName,
    //                                         const QString &savePath);

    void updateConnection(ConnectionInfo *connectionInfo);

    // --- Get methods ---
public:
    ClientConnectionManager &getClientConnectionManager() { return m_connectionManager; }
    DownloadManager &getDownloadManager() { return m_downloadManager; }

    DirectoryManager &getSystemDirectoryManager() { return m_systemDirectoryManager; }
    DirectoryManager &getNetworkDirectoryManager() { return m_networkDirectoryManager; }

    // --- Signal Slot Connection ---
private:
    ConnectionInfo *getActiveConnection();
    void setConnectionPreferences();

    QString generateDownloadID(int fileIndex);

    void connectSignals();
    void connectWebDavSignals();

signals:
    void connectionStatusCodeChanged(int statusCode);

    void currentDirectoryChanged(QList<FileInfo *> directoryList);
    void currentSystemDirectoryChanged();

    void fileReceived(const QString &filePath);

    void connected();
    void disconnected();

public slots:
    void onCurrentDirectoryChanged();
    void onActiveConnectionChanged();

    // --- Members ---
private:
    QHash<QString, ClientWebSocketMessenger *> m_websocketMessengers; ///< Connection name, Messenger
    QHash<QString, ClientTcpTlsTransport *> m_tcpTlsTransports;       ///< DownloadID, Transport

    QHash<QString, ClientWebDAV *> m_webDavMessengers; ///< Connection name, Messenger
    QHash<QString, ClientWebDAV *> m_webDavTransports; ///< DownloadID, Messenger

    ClientConnectionManager m_connectionManager; ///< Connection manager
    DownloadManager m_downloadManager;           ///< Download manager

    DirectoryManager m_systemDirectoryManager;
    DirectoryManager m_networkDirectoryManager;

    bool m_isActiveConnectionConnected{false};
};

#endif // CLIENT_HPP
