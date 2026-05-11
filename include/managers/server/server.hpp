#ifndef SERVER_HPP
#define SERVER_HPP

#include "server_http_messenger.hpp"
// #include "server_tcp_transport.hpp"

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    // ~Server() { stopServer(); }

    bool startServer();
    bool stopServer();

private:
    ServerMessenger *m_serverMessenger;        ///< Base server messenger class
    ServerHttpMessenger m_serverHttpMessenger; ///< Http messenger
    QString m_hostKey;
    // // ServerBluetoothMessenger *m_serverBluetoothMessenger;

    // ServerTransport *m_serverTransport;
    // ServerTcpTransport m_serverTcpTransport;
    // ServerUdpTransport *m_serverUdpTransport;
    // ServerBluetoothTransport *m_serverBluetoothTransport;
};

#endif // SERVER_HPP
