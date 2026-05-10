#ifndef SERVERHTTPMESSANGER_HPP
#define SERVERHTTPMESSANGER_HPP

#include <QSslServer>
#include <QTcpServer>
#include <QtHttpServer/QHttpServer>
#include "server_messenger.hpp"
#include <memory>

class ServerHttpMessenger : public ServerMessenger
{
    Q_OBJECT

public:
    explicit ServerHttpMessenger(const QString &hostKey,
                                 quint16 tcpPort = 6115,
                                 quint16 sslPort = 274,
                                 QObject *parent = nullptr);

    bool start(bool useDefaultConfig, bool useSecureConfig) override;
    void stop(bool stopDefaultConfig, bool stopSecureConfig) override;

private:
    void routeConnection();
    void routeFileSystem();
    void routeMissingHandler();

    template<typename Server>
    constexpr QStringView getProtocolName();
    template<typename Server>
    bool start(std::unique_ptr<Server> &server, quint16 port);
    template<typename Server>
    void stop(std::unique_ptr<Server> &server, quint16 port);

private:
    QHttpServer m_httpServer;

    std::unique_ptr<QTcpServer> m_tcpServer;
    std::unique_ptr<QSslServer> m_sslServer;

    QString m_hostKey;
    quint16 m_tcpPort;
    quint16 m_sslPort;
};

#endif // SERVERHTTPMESSANGER_HPP
