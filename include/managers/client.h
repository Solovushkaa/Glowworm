#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "clientsavedconnectionmanager.h"
#include "httpclient.h"
#include "unfinisheddownloadmanager.h"

using LocalClient = HttpClient;

// A class for combining the entire client part of the application.
// The class sends requests to the server and receives responses,
// as well as interacts with the GUI
class Client : public QObject
{
    Q_OBJECT
    /* Public class methods */
public:
    explicit Client(QObject *parent = nullptr);

    void checkConnectionToServer();

    void getDirectoryList(const QString &path, const QString &url, const QString &userID);
    void getFile(QList<QVariantHash> &currentDirectory,
                 QHash<QString, DownloadInfo> &downloadInfoDict,
                 const QString &path,
                 const QString &savePath,
                 const QString &saveName);

    void startDownload(const QString &downloadID);
    void stopDownload(const QString &downloadID);

    QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }

    /* Signal Slot Connection */
private:
    void signalSlotConnection();

    /* Members */
private:
    ClientSavedConnectionManager m_connManager;  // Менеджер для управления подключениями
    UnfinishedDownloadManager m_downloadManager; // Менеджер для управления загрузками

    LocalClient m_localClient; // HTTP/HTTPS клиент для работы в локальной сети
    // STUNClient              // UDP Клиент для подключения через STUN сервер
    // TURNClient              // HTTP/HTTPS клиент для пересылки данных через TURN-сервер
    // BluetoothClient         // Блютуз клиент для подключения и скачивания по блютуз

    BaseClient *m_currentNetworkInterface;

    // BluetoothConnector m_blueToothConnector; // возможно будет для подключения близких устройств

    QList<QVariantHash> m_currentDirectory; // Текущая выбранная директория

    /* Verifications */
private:
    void startConnectionVerification()
    {
        checkConnectionToServer();
        m_timer.start(m_pingTime);
    }
    void stopConnectionVerification() { m_timer.stop(); }

    QTimer m_timer;
    int m_pingTime = 5'000;
};

#endif // CLIENT_H
