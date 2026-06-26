#ifndef CLIENTWEBSOCKETMESSENGER_HPP
#define CLIENTWEBSOCKETMESSENGER_HPP

#include <QTimer>
#include <QWebSocket>
#include "connection_info.hpp"
#include "directory_manager.hpp"

Q_DECLARE_LOGGING_CATEGORY(client_websocket_messenger)

class ClientWebSocketMessenger : public QObject
{
    Q_OBJECT

public:
    ClientWebSocketMessenger(ConnectionInfo *connectionInfo,
                             const QByteArray &expectedFingerprint,
                             DirectoryManager &directoryManager,
                             QObject *parent = nullptr);
    ~ClientWebSocketMessenger();

    void connectToServer();
    void disconnectFromServer();

    void sendPing();
    void getNetworkDirectory(const QString &dirPath);

    // void getNetworkDirectory(ConnectionInfo *connectionInfo, const QString &dirPath);
    // void connectToRelayServer(ConnectionInfo *connectionInfo);

private:
    // void networkErrorHandler(const QNetworkReply::NetworkError networkError);

private slots:
    void onConnected();
    void onDisconnected();

    void onPong(quint64 elapsedTime, const QByteArray &payload);

    void onTextMessageReceived(const QString &message);

    void onError(QAbstractSocket::SocketError error);
    void onSslError(const QList<QSslError> &errors);

    // void onConnectionStatusCodeReceived();
    // void onDirectoryReceived();
    // void onRelayStatusCodeReceived();

signals:
    void connected(ConnectionInfo *connectionInfo);
    void disconnected(ConnectionInfo *connectionInfo);
    void errorOccurred(const QString &);

    void requestConnectionStatusReceivedError();
    void requestDirectoryReceivedError();

    void statusCodeChanged(int statusCode);
    void currentDirectoryChanged();

    /* Members */
private:
    ConnectionInfo *r_connectionInfo;
    QByteArray m_expectedFingerprint;

    DirectoryManager &r_directoryManager;

    QWebSocket m_socket;

    QString m_activeDirPath;

    // --- Verifications ---
private:
    QTimer m_pingTimer;

    int m_pongTimeout{3000}; // 3 sec
    int m_missedPongs{0};
};

#endif // CLIENTWEBSOCKETMESSENGER_HPP
