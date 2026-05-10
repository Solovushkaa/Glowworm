#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <QObject>
#include <QStandardPaths>
#include <QTimer>
#include "client_connection_manager.hpp"
#include "client_http_messenger.hpp"
#include "download_manager.hpp"
// #include "client_tcp_transport.hpp"

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
     * 
     * @param parent QObject parent
     */
    explicit Client(const QString &connectionsFilePath = QStandardPaths::writableLocation(
                                                             QStandardPaths::AppDataLocation)
                                                         + "/AppData/Client/SavedConnections.json",
                    const QString &downloadsFilePath = QStandardPaths::writableLocation(
                                                           QStandardPaths::AppDataLocation)
                                                       + "/AppData/Client/UnfinishedDownloads.json",
                    QObject *parent = nullptr);

    /**
     * @brief changeConnection
     */
    Q_INVOKABLE void changeConnection(int index);
    /**
     * @brief Checking the connection to the server.
     */
    void checkConnectionToServer();

    /**
     * @brief Getting a list of directories.
     * 
     * @param path Path to the requested directory
     * @param url URL to server
     * @param userID User ID
     */
    void getDirectoryList();
    /**
     * @brief Getting a file from the server.
     * 
     * @param currentDirectory 
     * @param downloadInfoDict
     * @param path
     * @param savePath
     * @param saveName
     */
    // void getFile(const QString &path, const QString &savePath, const QString &saveName);

    /**
     * @brief Start current download
     * @param downloadID Download ID
     */
    // void startDownload(const QString &downloadID);
    /**
     * @brief Stop current download
     * @param downloadID Download ID
     */
    // void stopDownload(const QString &downloadID);

    // QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }

    // --- Signal Slot Connection ---
private:
    void signalSlotConnection();

    // public slots:
    //     void onConnectionStatusCodeReceived(int statusCode)
    //     {
    //         emit connectionStatusCodeChanged(statusCode);
    //     }

signals:
    void connectionStatusCodeChanged(int statusCode);

    // --- Members ---
private:
    ClientMessenger *m_clientMessenger;
    ClientHttpMessenger m_clientHttpMessenger;

    ClientConnectionManager m_connectionManager; ///< Connection manager
    DownloadManager m_downloadManager;           ///< Download manager

    QList<QVariantHash> m_currentDirectory; ///< Current selected directory

    // BluetoothConnector m_blueToothConnector; ///< !Connector! Подключения близких устройств через BlueTooth
    // WiFiDirectConnector m_wifiDirectConnector; ///< !Connector! Подключения близких устройств через Wifi Direct

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
