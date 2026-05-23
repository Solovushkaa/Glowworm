#ifndef SERVER_HPP
#define SERVER_HPP

#include "server_http_messenger.hpp"
#include "server_tcp_transport.hpp"

Q_DECLARE_LOGGING_CATEGORY(server)

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    Q_INVOKABLE bool startDefaultServer();
    Q_INVOKABLE void stopDefaultServer();

private:
    QString m_hostKey;

    ServerHttpMessenger m_httpMessenger;
    ServerTcpTransport m_tcpTransport;

    bool m_defaultConfigEnabled{false};
    bool m_secureConfigEnabled{false};
};

#endif // SERVER_HPP
