#ifndef SERVER_HPP
#define SERVER_HPP

#include <QLoggingCategory>
#include "server_http_messenger.hpp"
// #include "server_tcp_transport.hpp"

Q_DECLARE_LOGGING_CATEGORY(server)

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    void setConfigPreferences(bool defaultConfigEnabled, bool secureConfigEnabled);

    bool startServers();
    bool stopServers();

    bool stopServer();

private:
    QString m_hostKey;

    ServerHttpMessenger m_httpMessenger; ///< Http messenger

    // ServerTcpTransport m_tcpTransport;
    // ServerUdpTransport m_udpTransport;

    bool m_httpMessengerEnabled{true};

    bool m_tcpTransportEnabled{true};
    bool m_udpTransportEnabled{true};

    bool m_defaultConfigEnabled{true};
    bool m_secureConfigEnabled{true};
};

#endif // SERVER_HPP
