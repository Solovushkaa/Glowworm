#ifndef SERVER_HPP
#define SERVER_HPP

#include "server_websocket_messenger.hpp"
#include "server_tcp_tls_transport.hpp"

Q_DECLARE_LOGGING_CATEGORY(server)

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    Q_INVOKABLE bool startServers();
    Q_INVOKABLE void stopServers();

    Q_INVOKABLE QString generateConnectionKey();

private:
    QString m_hostKey;

    ServerWebSocketMessenger m_webSocketMessenger;
    ServerTcpTlsTransport m_tcpTransport;
};

#endif // SERVER_HPP
