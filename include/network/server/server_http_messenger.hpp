#ifndef SERVERHTTPMESSANGER_HPP
#define SERVERHTTPMESSANGER_HPP

#include <QSslServer>
#include <QTcpServer>
#include <QtHttpServer/QHttpServer>
#include <memory>

class ServerHttpMessenger : public QObject
{
    Q_OBJECT

public:
    explicit ServerHttpMessenger(const QString &hostKey,
                                 quint16 tcpPort = 6115,
                                 quint16 sslPort = 274);

    bool startDefaultServer();
    void stopDefaultServer();

    bool startSecureServer();
    void stopSecureServer();

    bool startAll();
    void stopAll();

private:
    void routeConnection();
    void routeFileSystem();
    void routeMissingHandler();

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

    bool m_activeDefault{false};
    bool m_activeSecure{false};
};

#endif // SERVERHTTPMESSANGER_HPP
