#ifndef SERVERWEBSOCKETMESSANGER_HPP
#define SERVERWEBSOCKETMESSANGER_HPP

#include <QLoggingCategory>
#include <QWebSocketServer>

Q_DECLARE_LOGGING_CATEGORY(server_websocket_messenger)

class ServerWebSocketMessenger : public QObject
{
    Q_OBJECT

public:
    ServerWebSocketMessenger(const QString &hostKey, QObject *parent = nullptr);
    ~ServerWebSocketMessenger();

    bool start();
    void stop();

    quint16 getPort();

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onSocketDisconnected();

private:
    void processRequest(QWebSocket *sender, const QJsonObject &request);
    void sendResponse(QWebSocket *client, const QJsonObject &response);

private:
    QString m_hostKey;
    quint16 m_port;

    QWebSocketServer m_server;

    QList<QWebSocket *> m_clients; // Active sockets
};

#endif // SERVERWEBSOCKETMESSANGER_HPP
