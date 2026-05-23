#ifndef SERVERHTTPMESSANGER_HPP
#define SERVERHTTPMESSANGER_HPP

#include <QLoggingCategory>
#include <QSslServer>
#include <QTcpServer>
#include <QtHttpServer/QHttpServer>
#include <memory>

Q_DECLARE_LOGGING_CATEGORY(server_http_messenger)

class ServerHttpMessenger : public QObject
{
    Q_OBJECT

public:
    explicit ServerHttpMessenger(const QString &hostKey,
                                 quint16 tcpPort = 6115,
                                 quint16 sslPort = 274);
    ~ServerHttpMessenger();

    bool startDefault();
    void stopDefault();

    bool startSecure();
    void stopSecure();

private:
    void routeConnection();
    void routeFileSystem();
    void routeMissingHandler();

    void stopServer(bool stopSecretServer);

    template<typename Server>
    constexpr QStringView getProtocolName();
    template<typename Server>
    bool startServer(std::unique_ptr<Server> &server, quint16 port);
    template<typename Server>
    void stopServer(std::unique_ptr<Server> &server, quint16 port);

private:
    QString m_hostKey;
    quint16 m_tcpPort;
    quint16 m_sslPort;

    QHttpServer m_httpServer;

    std::unique_ptr<QTcpServer> m_tcpServer;
    std::unique_ptr<QSslServer> m_sslServer;

    bool m_defaultEnabled{false};
    bool m_secureEnabled{false};
};

#endif // SERVERHTTPMESSANGER_HPP
