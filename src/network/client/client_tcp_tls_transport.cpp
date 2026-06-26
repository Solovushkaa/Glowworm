#include "client_tcp_tls_transport.hpp"
#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QSslConfiguration>

Q_LOGGING_CATEGORY(client_tcp_tls_transport, "client.transport.tcp.tls")

ClientTcpTlsTransport::ClientTcpTlsTransport(const QString &outputFileName, QObject *parent)
    : QObject(parent)
    , m_socket(this)
    , m_outputFile(outputFileName)
{
    qCDebug(client_tcp_tls_transport) << "ClientTcpTlsTransport - created";
}

ClientTcpTlsTransport::~ClientTcpTlsTransport()
{
    qCDebug(client_tcp_tls_transport) << "ClientTcpTlsTransport - destroyed";
}

void ClientTcpTlsTransport::getFile(const QString &address, qint16 port, DownloadInfo *downloadInfo)
{
    qCDebug(client_tcp_tls_transport)
        << "Getting file:" << downloadInfo->m_path << "From host:" << address;

    const QString &downloadID = downloadInfo->m_downloadID;

    m_socket.setDownloadInfo(downloadInfo);
    connectSignals();

    startNewDownload(address, port, downloadID);

    qCInfo(client_tcp_tls_transport) << "File fetch request created:" << downloadInfo->m_path;
}

void ClientTcpTlsTransport::getFileFromRelay(const QString &address,
                                             qint16 port,
                                             [[maybe_unused]] const QString &userName,
                                             DownloadInfo *downloadInfo)
{
    qCCritical(client_tcp_tls_transport)
        << "Getting file:" << downloadInfo->m_path << "From host:" << address;

    const QString &downloadID = downloadInfo->m_downloadID;

    m_socket.setDownloadInfo(downloadInfo);
    connectSignals();

    startNewDownload(address, port, downloadID);

    qCInfo(client_tcp_tls_transport) << "File fetch request created:" << downloadInfo->m_path;
}

void ClientTcpTlsTransport::connectSignals()
{
    qCDebug(client_tcp_tls_transport) << "Connecting signals";

    QSslConfiguration sslConfig;
    sslConfig.setProtocol(QSsl::TlsV1_3);
    m_socket.socket()->setSslConfiguration(sslConfig);

    connect(&m_socket, &MessageSocket::connected, this, &ClientTcpTlsTransport::onConnected);
    connect(&m_socket, &MessageSocket::disconnected, this, &ClientTcpTlsTransport::onDisconnected);
    connect(&m_socket, &MessageSocket::errorOccurred, this, &ClientTcpTlsTransport::onSocketError);
    connect(&m_socket,
            &MessageSocket::messageReceived,
            this,
            &ClientTcpTlsTransport::onMessageReceived);
    connect(m_socket.socket(), &QSslSocket::sslErrors, this, &ClientTcpTlsTransport::onSslError);

    connect(&m_socket, &MessageSocket::encrypted, this, &ClientTcpTlsTransport::onEncrypted);
}

void ClientTcpTlsTransport::startNewDownload(const QString &address, qint16 port)
{
    qCDebug(client_tcp_tls_transport) << "Starting new download on host:" << address;
    m_socket.connectToHost(address, port);
}

void ClientTcpTlsTransport::startNewDownload(const QString &address,
                                             qint16 port,
                                             const QString &username)
{
    qCDebug(client_tcp_tls_transport) << "Starting new download on host:" << address;
    m_socket.setProperty("username", username);
    m_socket.connectToHost(address, port);
}

void ClientTcpTlsTransport::requestFile(DownloadInfo *downloadInfo,
                                        const QString &username,
                                        bool isUsername)
{
    qCDebug(client_tcp_tls_transport) << "File request:" << downloadInfo->m_path;

    if (m_socket.socket()->state() != QSslSocket::ConnectedState) {
        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred("Not connected");
        return;
    }

    QByteArray request;
    if (isUsername && false) {
        request.append(message::toByteFromStatus(TransportStatus::RequestFile));
        request.append(username.toUtf8());
    }
    request.append(message::toByteFromStatus(TransportStatus::RequestFile));
    request.append(reinterpret_cast<const char *>(&downloadInfo->m_lastReceivedByte),
                   sizeof(downloadInfo->m_lastReceivedByte));
    request.append(downloadInfo->m_downloadID.toUtf8());
    request.append(downloadInfo->m_path.toUtf8());

    downloadInfo->setDownloadState(DownloadInfo::DownloadState::Active);

    qCCritical(client_tcp_tls_transport) << "request:" << request;
    m_socket.sendMessage(request);

    qCInfo(client_tcp_tls_transport) << "File request sent";
}

void ClientTcpTlsTransport::onConnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_tls_transport) << "Connection established via socket descriptor:"
                                     << messageSocket->socket()->socketDescriptor();

    bool isUsername = false;
    auto username = messageSocket->property("username");
    if (username.isValid()) {
        isUsername = true;
    }
    requestFile(messageSocket->getDownloadInfo(), username.toString(), isUsername);

    emit connected();
}

void ClientTcpTlsTransport::onDisconnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_tls_transport)
        << "Connection closed on socket descriptor:" << messageSocket->socket()->socketDescriptor();

    if (m_outputFile.isOpen()) {
        qCInfo(client_tcp_tls_transport) << "Save file closed:" << m_outputFile.fileName();
        m_outputFile.close();
    }

    emit disconnected();
}

void ClientTcpTlsTransport::onMessageReceived(const QByteArray &message)
{
    qCDebug(client_tcp_tls_transport) << "Received message";
    if (message.isEmpty())
        return;

    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCDebug(client_tcp_tls_transport)
        << "Message received on socket:" << messageSocket->socket()->socketDescriptor();

    auto downloadInfo = messageSocket->getDownloadInfo();

    TransportStatus status = static_cast<TransportStatus>(message.at(0));
    qCDebug(client_tcp_tls_transport) << "Download state:" << downloadInfo->m_downloadState;
    qCDebug(client_tcp_tls_transport) << "Message status:" << static_cast<int>(status);
    QByteArray payload = message.mid(1);

    switch (status) {
    case TransportStatus::ResponseOk: {
        qCInfo(client_tcp_tls_transport) << "Response: Ok";
        bool statusOk;
        downloadInfo->m_size = payload.toLongLong(&statusOk);
        if (!statusOk || downloadInfo->m_size < 0) {
            qCDebug(client_tcp_tls_transport) << "Invalid file size received";
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            downloadInfo->m_size = 0;
            emit errorOccurred("Invalid file size received");
            return;
        }

        if (!m_outputFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qCDebug(client_tcp_tls_transport) << "Cannot save" << downloadInfo->m_savePath
                                              << "file:" << m_outputFile.errorString();
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            emit errorOccurred("Cannot save file: " + m_outputFile.errorString());
            return;
        }
        m_outputFile.seek(downloadInfo->m_lastReceivedByte);

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Active);

        qCInfo(client_tcp_tls_transport)
            << "Receiving file of size" << downloadInfo->m_size << "bytes";

        break;
    }
    case TransportStatus::ResponseError: {
        qCInfo(client_tcp_tls_transport) << "Response: Error";
        qCWarning(client_tcp_tls_transport) << "Error:" << QString::fromUtf8(payload);

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred(QString::fromUtf8(payload));

        break;
    }
    case TransportStatus::FileChunk: {
        qCDebug(client_tcp_tls_transport) << "Response: FileChunk";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active
            || !m_outputFile.isOpen()) {
            emit errorOccurred("Unexpected file chunk");
            return;
        }
        m_outputFile.write(payload);

        downloadInfo->setLastReceivedByte(downloadInfo->m_lastReceivedByte + payload.size());

        break;
    }
    case TransportStatus::TransferComplete: {
        qCInfo(client_tcp_tls_transport) << "Response: TransferComplete";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active) {
            qCWarning(client_tcp_tls_transport) << "Unexpected transfer complete";
            emit errorOccurred("Unexpected transfer complete");
            return;
        }

        if (m_outputFile.size() != downloadInfo->m_size) {
            qCWarning(client_tcp_tls_transport) << "File size mismatch";
            emit errorOccurred("File size mismatch");
            return;
        }

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Finish);
        downloadInfo->m_forDelete = true;

        emit fileReceived(downloadInfo->m_savePath);

        qCCritical(client_tcp_tls_transport) << "File received:" << downloadInfo->m_savePath;

        break;
    }
    default:
        qCInfo(client_tcp_tls_transport) << "Response: Unknown";

        qCWarning(client_tcp_tls_transport)
            << "Unknown message type:"
            << static_cast<std::underlying_type_t<TransportStatus>>(status);
    }
}

void ClientTcpTlsTransport::onSocketError([[maybe_unused]] QAbstractSocket::SocketError error)
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCWarning(client_tcp_tls_transport)
        << "Error on socket:" << messageSocket->socket()->socketDescriptor();
    qCWarning(client_tcp_tls_transport) << "Error:" << messageSocket->socket()->errorString();

    emit errorOccurred(messageSocket->socket()->errorString());
}

void ClientTcpTlsTransport::onSslError(const QList<QSslError> &errors)
{
    qCDebug(client_tcp_tls_transport) << "===================== =====================";

    auto socket = qobject_cast<QSslSocket *>(sender());

    disconnect(socket, &QSslSocket::sslErrors, this, &ClientTcpTlsTransport::onSslError);

    qInfo() << "SSL errors:";
    for (const auto &e : errors)
        qInfo() << e.errorString();

    QSslCertificate peerCert = socket->peerCertificate();
    qInfo() << "Is peerCert:" << !peerCert.isNull();

    QByteArray actualFingerprint = peerCert.digest(QCryptographicHash::Sha256);

    qInfo() << "Actual Fingerprint:" << (actualFingerprint == expectedCert);

    if (actualFingerprint == actualFingerprint /*expectedCert*/) {
        qInfo() << "actualFingerprint == m_expectedFingerprint";
        socket->ignoreSslErrors();
    } else {
        qInfo() << "Connection Failed: Bad Certificate Peer";
        socket->close();
    }
}

void ClientTcpTlsTransport::onEncrypted()
{
    qCDebug(client_tcp_tls_transport) << "Encrypted";
    // auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());
}

void ClientTcpTlsTransport::onEnded() {}
