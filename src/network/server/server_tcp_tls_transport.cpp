#include "server_tcp_tls_transport.hpp"
#include <QDir>
#include <QSettings>
#include <QSslKey>
#include "constants.hpp"
#include "file_sender.hpp"

Q_LOGGING_CATEGORY(server_tcp_tls_transport, "server.transport.tcp.tls")

ServerTcpTlsTransport::ServerTcpTlsTransport(QObject *parent)
    : QObject(parent)
    , m_server(new QSslServer(this))
{
    // timer.singleShot(5000, this, &ServerTcpTlsTransport::onEnded);

    QSettings settings;
    m_port = settings.value(constants::kTransportPortName).toInt();

    const QString serverCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    const QString serverKeyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/appdata/server/server_key.key";

    QFile serverCertificate(serverCertPath);
    QFile serverPrivateKey(serverKeyPath);
    if (!serverCertificate.open(QIODevice::ReadOnly)) {
        qFatal("Failed to load generated key/cert");
    }
    if (!serverPrivateKey.open(QIODevice::ReadOnly)) {
        qFatal("Failed to load generated key/cert");
    }

    QSslCertificate sslCert(serverCertificate.readAll(), QSsl::Pem);
    QSslKey sslKey(serverPrivateKey.readAll(), QSsl::Ec, QSsl::Pem);

    if (sslCert.isNull() || sslKey.isNull()) {
        qFatal("Failed to load generated key/cert");
    }

    QSslConfiguration sslConfig;
    sslConfig.setLocalCertificate(sslCert);
    sslConfig.setPrivateKey(sslKey);
    sslConfig.setProtocol(QSsl::TlsV1_3);

    m_server->setSslConfiguration(sslConfig);

    connect(m_server,
            &QSslServer::pendingConnectionAvailable,
            this,
            &ServerTcpTlsTransport::onNewConnection);

    connect(m_server, &QSslServer::errorOccurred, this, &ServerTcpTlsTransport::onErrorOccured);
    connect(m_server, &QSslServer::peerVerifyError, this, &ServerTcpTlsTransport::onPeerVerifyError);
    connect(m_server, &QSslServer::sslErrors, this, &ServerTcpTlsTransport::onSslErrors);

    qCDebug(server_tcp_tls_transport) << "ServerTcpTlsTransport - created";
}

void ServerTcpTlsTransport::onErrorOccured(QSslSocket *, QAbstractSocket::SocketError err)
{
    qDebug() << "SSL SERVER ERROR:" << err;
}

void ServerTcpTlsTransport::onPeerVerifyError(QSslSocket *, const QSslError &err)
{
    qDebug() << "PEER VERIFY ERROR:" << err.errorString();
}

void ServerTcpTlsTransport::onSslErrors(QSslSocket *, const QList<QSslError> &errs)
{
    qDebug() << "SERVER SSL ERRORS";
    for (auto &e : errs)
        qDebug() << e.errorString();
}

ServerTcpTlsTransport::~ServerTcpTlsTransport()
{
    stop();
    qCDebug(server_tcp_tls_transport) << "ServerTcpTlsTransport - destroyed";
}

bool ServerTcpTlsTransport::start()
{
    qCDebug(server_tcp_tls_transport) << "Starting ServerTcpTlsTransport";

    if (m_enabled) {
        qCInfo(server_tcp_tls_transport) << "TCP transport server is already running";
        return true;
    }

    if (!m_server->listen(QHostAddress::Any, m_port)) {
        qCCritical(server_tcp_tls_transport) << "Server error:" << m_server->errorString();
        return false;
    }

    qCInfo(server_tcp_tls_transport) << "Server listening on port" << m_port;

    return true;
}

void ServerTcpTlsTransport::stop()
{
    qCDebug(server_tcp_tls_transport) << "Stopping ServerTcpTlsTransport";

    if (m_enabled) {
        qCInfo(server_tcp_tls_transport) << "TCP transport server has already stopped";
        return;
    }

    m_server->close();
    for (MessageSocket *client : std::as_const(m_clients)) {
        client->deleteLater();
    }
    m_clients.clear();
}

quint16 ServerTcpTlsTransport::getPort()
{
    return m_port;
}

void ServerTcpTlsTransport::onNewConnection()
{
    qCDebug(server_tcp_tls_transport) << "New connection detected";

    // qDebug() << m_server->nextPendingConnection();
    qDebug() << "hasPendingConnections=" << m_server->hasPendingConnections();
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        qCDebug(server_tcp_tls_transport) << "socket =" << socket;
        qCDebug(server_tcp_tls_transport) << "meta =" << socket->metaObject()->className();
        QSslSocket *bad = qobject_cast<QSslSocket *>(socket);
        auto *client = new MessageSocket(bad, this);

        connect(client,
                &MessageSocket::messageReceived,
                this,
                &ServerTcpTlsTransport::onClientMessage);
        connect(client,
                &MessageSocket::disconnected,
                this,
                &ServerTcpTlsTransport::onClientDisconnected);

        m_clients.append(client);

        qCInfo(server_tcp_tls_transport) << "Client connected:" << socket->peerAddress().toString();
    }
    qCDebug(server_tcp_tls_transport) << "New connection ended";
}

void ServerTcpTlsTransport::onClientMessage(const QByteArray &message)
{
    qCDebug(server_tcp_tls_transport) << "New client message detected:" << message.size() << "B";
    qCDebug(server_tcp_tls_transport) << "New client message detected:" << message;

    auto *client = qobject_cast<MessageSocket *>(sender());
    if (!client || message.isEmpty())
        return;

    TransportStatus status = static_cast<TransportStatus>(message[0]);

    if (status == TransportStatus::RequestFile) {
        int statusLength = sizeof(int8_t);
        int readOffsetLength = sizeof(qint64);

        qint64 readOffset = message.mid(statusLength, readOffsetLength).toLongLong();
        qCDebug(server_tcp_tls_transport) << "File read offset:" << readOffset;

        QString downloadID = message.mid(statusLength + readOffsetLength,
                                         constants::kDownloadIDLength);
        QString filePath = QString::fromUtf8(
            message.mid(statusLength + readOffsetLength + constants::kDownloadIDLength));

        qCDebug(server_tcp_tls_transport) << "Request file:" << filePath;
        // if (!isAvailable(fileName)) {
        //     QByteArray err;
        //     err.append(message::toByteFromStatus(TransportStatus::ResponseError));
        //     err.append("Access denied");

        //     client->sendMessage(err);

        //     return;
        // }

        auto *sender = new FileSender(client, downloadID, filePath, readOffset, this);
        connect(sender, &FileSender::finished, this, &ServerTcpTlsTransport::onFileSendFinished);
        sender->start();
    }
}

void ServerTcpTlsTransport::onClientDisconnected()
{
    qCDebug(server_tcp_tls_transport) << "Disconnecting the client";

    auto *client = qobject_cast<MessageSocket *>(sender());

    if (client) {
        cleanupClient(client);
    }
}

void ServerTcpTlsTransport::cleanupClient(MessageSocket *client)
{
    m_clients.removeOne(client);

    client->deleteLater();

    qCInfo(server_tcp_tls_transport)
        << "Client disconnected:" << client->socket()->peerAddress().toString();
}

void ServerTcpTlsTransport::onFileSendFinished()
{
    auto fileSender = qobject_cast<FileSender *>(sender());
    fileSender->deleteLater();
}

void ServerTcpTlsTransport::onEnded()
{
    qCInfo(server_tcp_tls_transport) << "Server Closed";
    m_server->close();
}
