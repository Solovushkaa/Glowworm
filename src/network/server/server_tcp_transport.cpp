#include "server_tcp_transport.hpp"
#include <QDir>
#include <QTcpServer>
#include "constants.hpp"
#include "file_sender.hpp"

Q_LOGGING_CATEGORY(server_tcp_transport, "server.transport.tcp")

ServerTcpTransport::ServerTcpTransport(quint16 port, QObject *parent)
    : QObject(parent)
    , m_port(port)
    , m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &ServerTcpTransport::onNewConnection);

    qCDebug(server_tcp_transport) << "ServerTcpTransport - created";
}

ServerTcpTransport::~ServerTcpTransport()
{
    stop();
    qCDebug(server_tcp_transport) << "ServerTcpTransport - destroyed";
}

bool ServerTcpTransport::start()
{
    qCDebug(server_tcp_transport) << "Starting ServerTcpTransport";

    if (m_enabled) {
        qCInfo(server_tcp_transport) << "TCP transport server is already running";
        return true;
    }

    if (!m_server->listen(QHostAddress::Any, m_port)) {
        qCCritical(server_tcp_transport) << "Server error:" << m_server->errorString();
        return false;
    }

    qCInfo(server_tcp_transport) << "Server listening on port" << m_port;

    return true;
}

void ServerTcpTransport::stop()
{
    qCDebug(server_tcp_transport) << "Stopping ServerTcpTransport";

    if (m_enabled) {
        qCInfo(server_tcp_transport) << "TCP transport server has already stopped";
        return;
    }

    m_server->close();
    for (MessageSocket *client : std::as_const(m_clients)) {
        client->deleteLater();
    }
    m_clients.clear();
}

void ServerTcpTransport::onNewConnection()
{
    qCDebug(server_tcp_transport) << "New connection detected";

    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        auto *client = new MessageSocket(socket, this);

        connect(client, &MessageSocket::messageReceived, this, &ServerTcpTransport::onClientMessage);
        connect(client,
                &MessageSocket::disconnected,
                this,
                &ServerTcpTransport::onClientDisconnected);

        m_clients.append(client);

        qCInfo(server_tcp_transport) << "Client connected:" << socket->peerAddress().toString();
    }
}

void ServerTcpTransport::onClientMessage(const QByteArray &message)
{
    qCDebug(server_tcp_transport) << "New client message detected:" << message.size() << "B";

    auto *client = qobject_cast<MessageSocket *>(sender());
    if (!client || message.isEmpty())
        return;

    TransportStatus status = static_cast<TransportStatus>(message[0]);

    if (status == TransportStatus::RequestFile) {
        int statusLength = sizeof(int8_t);
        int readOffsetLength = sizeof(qint64);

        qint64 readOffset = message.mid(statusLength, readOffsetLength).toLongLong();
        qCDebug(server_tcp_transport) << "File read offset:" << readOffset;

        QString downloadID = message.mid(statusLength + readOffsetLength,
                                         constants::kDownloadIDLength);
        QString filePath = QString::fromUtf8(
            message.mid(statusLength + readOffsetLength + constants::kDownloadIDLength));

        qCDebug(server_tcp_transport) << "Request file:" << filePath;
        // if (!isAvailable(fileName)) {
        //     QByteArray err;
        //     err.append(message::toByteFromStatus(TransportStatus::ResponseError));
        //     err.append("Access denied");

        //     client->sendMessage(err);

        //     return;
        // }

        auto *sender = new FileSender(client, downloadID, filePath, readOffset, this);
        connect(sender, &FileSender::finished, this, &ServerTcpTransport::onFileSendFinished);
        sender->start();
    }
}

void ServerTcpTransport::onClientDisconnected()
{
    qCDebug(server_tcp_transport) << "Disconnecting the client";

    auto *client = qobject_cast<MessageSocket *>(sender());

    if (client) {
        cleanupClient(client);
    }
}

void ServerTcpTransport::cleanupClient(MessageSocket *client)
{
    m_clients.removeOne(client);

    client->deleteLater();

    qCInfo(server_tcp_transport) << "Client disconnected:"
                                 << client->socket()->peerAddress().toString();
}

void ServerTcpTransport::onFileSendFinished()
{
    auto fileSender = qobject_cast<FileSender *>(sender());
    fileSender->deleteLater();
}
