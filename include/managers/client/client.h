#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "clientconnectionmanager.h"
#include "directclient.h"
#include "downloadmanager.h"

struct ClientNetworkProtocolFactory
{
    std::unique_ptr<BaseClient> create(Transport transport)
    {
        using enum Transport;
        using std::make_unique;

        switch (transport) {
        case DIRECT:
            return make_unique<DirectClient>();
            break;
        case STUN:
            // return make_unique<STUNClient>();
            break;
        case BLUETOOTH:
            // return make_unique<BluetoothClient>();
            break;
        case TURN:
            // return make_unique<TURNClient>();
            break;
        }
        return {};
    }
};

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
    explicit Client(QObject *parent = nullptr);

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
    void getDirectoryList(const QString &path, const QString &url, const QString &userID);
    /**
     * @brief Getting a file from the server.
     * 
     * @param currentDirectory 
     * @param downloadInfoDict
     * @param path
     * @param savePath
     * @param saveName
     */
    void getFile(const QString &path, const QString &savePath, const QString &saveName);

    /**
     * @brief Start current download
     * @param downloadID Download ID
     */
    void startDownload(const QString &downloadID);
    /**
     * @brief Stop current download
     * @param downloadID Download ID
     */
    void stopDownload(const QString &downloadID);

    QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }

    // --- Signal Slot Connection ---
private:
    void signalSlotConnection();

    // --- Members ---
private:
    ClientConnectionManager m_connectionManager; ///< Connection manager
    DownloadManager m_downloadManager;          ///< Download manager

    ClientNetworkProtocolFactory m_protocolFactory; ///< Фабрика для создания сетевых транспортов
    BaseClient *m_currentClientNetworkProtocol;     ///< Selected router

    // LifeController m_lifeController; ///< Контроллер за временем жизни подключений, которые ещё не завершены
    // (подключения передаются после смены текущего протокола)

    QList<QVariantHash> m_currentDirectory; ///< Current selected directory

    // DirectClient m_httpClient; ///< HTTP/HTTPS клиент для работы в локальной сети
    // BluetoothClient         ///< Блютуз клиент для подключения и скачивания по блютуз
    // STUNClient              ///< UDP Клиент для подключения через STUN сервер
    // TURNClient              ///< HTTP/HTTPS клиент для пересылки данных через TURN-сервер

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

#endif // CLIENT_H
