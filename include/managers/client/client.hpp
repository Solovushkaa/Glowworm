#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QStandardPaths>
#include <QTimer>
#include "client_connection_manager.hpp"
#include "client_http_messenger.hpp"
#include "client_tcp_transport.hpp"
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
    Q_INVOKABLE void checkConnectionToServer();

    /**
     * @brief Getting a list of directories.
     */
    Q_INVOKABLE void getDirectory(const QString &dirPath);
    /**
     * @brief Getting a file from the server.
     */
    Q_INVOKABLE void getFile(int fileIndex);
    /**
     * @brief Start current download
     * @param downloadID Download ID
     */
    // Q_INVOKABLE void startDownload(int downloadIndex);
    /**
     * @brief Stop current download
     * @param downloadID Download ID
     */
    // Q_INVOKABLE void stopDownload(int downloadIndex);

    // --- Signal Slot Connection ---
private:
    ConnectionInfo *getActiveConnection();
    void setConnectionPreferences();

    void signalSlotConnection();

signals:
    void connectionStatusCodeChanged(int statusCode);
    void currentDirectoryChanged(QList<FileInfo *> directoryList);

public slots:
    void onCurrentDirectoryChanged();

    // --- Members ---
private:
    ClientHttpMessenger m_httpMessenger;

    ClientTcpTransport m_tcpTransport;

    ClientConnectionManager m_connectionManager; ///< Connection manager
    DownloadManager m_downloadManager;           ///< Download manager

    DirectoryManager m_directoryManager;

    // --- Verifications ---
private:
    void startConnectionVerification()
    {
        checkConnectionToServer();
        m_timer.start(m_pingTime);
    }
    void stopConnectionVerification() { m_timer.stop(); }

    QTimer m_timer;        ///< Таймер для проверки доступа к серверу
    int m_pingTime{5'000}; ///< Время между опросом сервера (в секундах)
};

#endif // CLIENT_HPP
