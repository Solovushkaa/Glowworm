#ifndef SERVERTCPTRANSPORT_HPP
#define SERVERTCPTRANSPORT_HPP

#include <QList>
#include <QObject>
#include <QTcpServer>
#include "message_socket.hpp"

Q_DECLARE_LOGGING_CATEGORY(server_tcp_transport)

class ServerTcpTransport : public QObject
{
    Q_OBJECT

public:
    explicit ServerTcpTransport(quint16 port = 6821, QObject *parent = nullptr);
    ~ServerTcpTransport();

public:
    bool start();
    void stop();

private slots:
    void onNewConnection();
    void onClientMessage(const QByteArray &message);
    void onClientDisconnected();
    void onFileSendFinished();

private:
    void cleanupClient(MessageSocket *client);

private:
    quint16 m_port;

    QTcpServer *m_server;
    QList<MessageSocket *> m_clients;
};

#endif // SERVERTCPTRANSPORT_HPP
