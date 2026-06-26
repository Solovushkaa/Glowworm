#ifndef SERVERTCPTLSTRANSPORT_HPP
#define SERVERTCPTLSTRANSPORT_HPP

#include <QList>
#include <QObject>
#include <QSslServer>
#include "message_socket.hpp"

Q_DECLARE_LOGGING_CATEGORY(server_tcp_tls_transport)

class ServerTcpTlsTransport : public QObject
{
    Q_OBJECT

public:
    explicit ServerTcpTlsTransport(QObject *parent = nullptr);
    ~ServerTcpTlsTransport();

public:
    bool start();
    void stop();

    quint16 getPort();

private slots:
    void onNewConnection();
    void onClientMessage(const QByteArray &message);
    void onClientDisconnected();
    void onFileSendFinished();
    void onEnded();

    void onErrorOccured(QSslSocket *socket, QAbstractSocket::SocketError err);
    void onPeerVerifyError(QSslSocket *socket, const QSslError &err);
    void onSslErrors(QSslSocket *socket, const QList<QSslError> &errs);

private:
    void cleanupClient(MessageSocket *client);

private:
    quint16 m_port;

    QSslServer *m_server;
    QList<MessageSocket *> m_clients;

    bool m_enabled{false};
};

#endif // SERVERTCPTLSTRANSPORT_HPP
