#include "client_tls_transport.hpp"
#include <QDataStream>
#include <QDir>
#include <QFileInfo>

Q_LOGGING_CATEGORY(client_tcp_transport, "client.transport.tcp")

ClientTcpTransport::ClientTcpTransport(QObject *parent)
    : QObject(parent)
{
    qCDebug(client_tcp_transport) << "ClientTcpTransport - created";
}

ClientTcpTransport::~ClientTcpTransport()
{
    qCDebug(client_tcp_transport) << "ClientTcpTransport - destroyed";
}

void ClientTcpTransport::getFile(const QString &address, qint16 port, DownloadInfo *downloadInfo)
{
    qCDebug(client_tcp_transport) << "Getting file:" << downloadInfo->m_path
                                  << "From host:" << address;

    const QString &downloadID = downloadInfo->m_downloadID;

    m_sockets[downloadID] = new ClientMessageSocket(this);
    m_sockets[downloadID]->setDownloadInfo(downloadInfo);
    connectSignals(m_sockets[downloadID]);

    startNewDownload(address, port, downloadID);

    qCInfo(client_tcp_transport) << "File fetch request created:" << downloadInfo->m_path;
}

void ClientTcpTransport::getFileFromRelay(const QString &address,
                                          qint16 port,
                                          [[maybe_unused]] const QString &userName,
                                          DownloadInfo *downloadInfo)
{
    qCCritical(client_tcp_transport)
        << "Getting file:" << downloadInfo->m_path << "From host:" << address;

    const QString &downloadID = downloadInfo->m_downloadID;

    m_sockets[downloadID] = new ClientMessageSocket(this);
    m_sockets[downloadID]->setDownloadInfo(downloadInfo);
    connectSignals(m_sockets[downloadID]);

    startNewDownload(address, port, downloadID);

    qCInfo(client_tcp_transport) << "File fetch request created:" << downloadInfo->m_path;
}

void ClientTcpTransport::connectSignals(ClientMessageSocket *messenger)
{
    qCDebug(client_tcp_transport) << "Connecting signals";

    connect(messenger, &MessageSocket::connected, this, &ClientTcpTransport::onConnected);
    connect(messenger, &MessageSocket::disconnected, this, &ClientTcpTransport::onDisconnected);
    connect(messenger, &MessageSocket::errorOccurred, this, &ClientTcpTransport::onSocketError);
    connect(messenger,
            &MessageSocket::messageReceived,
            this,
            &ClientTcpTransport::onMessageReceived);
}

void ClientTcpTransport::startNewDownload(const QString &address,
                                          qint16 port,
                                          const QString &downloadID)
{
    qCDebug(client_tcp_transport) << "Starting new download on host:" << address;
    m_sockets[downloadID]->connectToHost(address, port);
}

void ClientTcpTransport::startNewDownload(const QString &address,
                                          qint16 port,
                                          const QString &username,
                                          const QString &downloadID)
{
    qCDebug(client_tcp_transport) << "Starting new download on host:" << address;
    m_sockets[downloadID]->setProperty("username", username);
    m_sockets[downloadID]->connectToHost(address, port);
}

void ClientTcpTransport::requestFile(DownloadInfo *downloadInfo,
                                     const QString &username,
                                     bool isUsername)
{
    qCDebug(client_tcp_transport) << "File request:" << downloadInfo->m_path;

    auto socket = m_sockets[downloadInfo->m_downloadID];

    if (socket->socket()->state() != QTcpSocket::ConnectedState) {
        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred("Not connected");
        return;
    }

    QByteArray request;
    if (isUsername) {
        request.append(message::toByteFromStatus(TransportStatus::RequestFile));
        request.append(username.toUtf8());
    }
    request.append(message::toByteFromStatus(TransportStatus::RequestFile));
    request.append(reinterpret_cast<const char *>(&downloadInfo->m_lastReceivedByte),
                   sizeof(downloadInfo->m_lastReceivedByte));
    request.append(downloadInfo->m_downloadID.toUtf8());
    request.append(downloadInfo->m_path.toUtf8());

    downloadInfo->setDownloadState(DownloadInfo::DownloadState::Active);

    socket->sendMessage(request);

    qCInfo(client_tcp_transport) << "File request sent";
}

void ClientTcpTransport::onConnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_transport) << "Connection established via socket descriptor:"
                                 << messageSocket->socket()->socketDescriptor();

    bool isUsername = false;
    auto username = messageSocket->property("username");
    if (username.isValid()) {
        isUsername = true;
    }
    requestFile(messageSocket->getDownloadInfo(), username.toString(), isUsername);

    emit connected();
}

void ClientTcpTransport::onDisconnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_transport) << "Connection closed on socket descriptor:"
                                 << messageSocket->socket()->socketDescriptor();

    const QString &downloadID = messageSocket->getDownloadInfo()->m_downloadID;
    if (m_outputFiles[downloadID]->isOpen()) {
        qCInfo(client_tcp_transport)
            << "Save file closed:" << m_outputFiles[downloadID]->fileName();

        m_outputFiles[downloadID]->deleteLater();
        m_outputFiles.remove(downloadID);        
    }

    emit disconnected();
}

void ClientTcpTransport::onMessageReceived(const QByteArray &message)
{
    qCDebug(client_tcp_transport) << "Received message";
    if (message.isEmpty())
        return;

    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCDebug(client_tcp_transport) << "Message received on socket:"
                                  << messageSocket->socket()->socketDescriptor();

    auto downloadInfo = messageSocket->getDownloadInfo();
    auto downloadID = downloadInfo->m_downloadID;

    TransportStatus status = static_cast<TransportStatus>(message.at(0));
    qCDebug(client_tcp_transport) << "Download state:" << downloadInfo->m_downloadState;
    qCDebug(client_tcp_transport) << "Message status:" << static_cast<int>(status);
    QByteArray payload = message.mid(1);

    switch (status) {
    case TransportStatus::ResponseOk: {
        qCInfo(client_tcp_transport) << "Response: Ok";
        bool statusOk;
        downloadInfo->m_size = payload.toLongLong(&statusOk);
        if (!statusOk || downloadInfo->m_size < 0) {
            qCDebug(client_tcp_transport) << "Invalid file size received";
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            downloadInfo->m_size = 0;
            emit errorOccurred("Invalid file size received");
            return;
        }

        m_outputFiles.insert(downloadID, new QFile(downloadInfo->m_savePath, this));
        if (!m_outputFiles[downloadID]->open(QIODevice::WriteOnly | QIODevice::Append)) {
            qCDebug(client_tcp_transport) << "Cannot save" << downloadInfo->m_savePath
                                          << "file:" << m_outputFiles[downloadID]->errorString();
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            emit errorOccurred("Cannot save file: " + m_outputFiles[downloadID]->errorString());
            return;
        }
        m_outputFiles[downloadID]->seek(downloadInfo->m_lastReceivedByte);

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Active);

        qCInfo(client_tcp_transport) << "Receiving file of size" << downloadInfo->m_size << "bytes";

        break;
    }
    case TransportStatus::ResponseError: {
        qCInfo(client_tcp_transport) << "Response: Error";
        qCWarning(client_tcp_transport) << "Error:" << QString::fromUtf8(payload);

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred(QString::fromUtf8(payload));

        break;
    }
    case TransportStatus::FileChunk: {
        qCDebug(client_tcp_transport) << "Response: FileChunk";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active
            || !m_outputFiles[downloadID]->isOpen()) {
            emit errorOccurred("Unexpected file chunk");
            return;
        }
        m_outputFiles[downloadID]->write(payload);

        downloadInfo->setLastReceivedByte(downloadInfo->m_lastReceivedByte + payload.size());

        break;
    }
    case TransportStatus::TransferComplete: {
        qCInfo(client_tcp_transport) << "Response: TransferComplete";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active) {
            qCWarning(client_tcp_transport) << "Unexpected transfer complete";
            emit errorOccurred("Unexpected transfer complete");
            return;
        }

        if (m_outputFiles[downloadID]->size() != downloadInfo->m_size) {
            qCWarning(client_tcp_transport) << "File size mismatch";
            emit errorOccurred("File size mismatch");
            return;
        }

        m_outputFiles[downloadID]->deleteLater();
        m_outputFiles.remove(downloadID);
        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Finish);
        downloadInfo->m_forDelete = true;

        emit fileReceived(downloadInfo->m_savePath);

        qCInfo(client_tcp_transport) << "File received:" << downloadInfo->m_savePath;

        break;
    }
    default:
        qCInfo(client_tcp_transport) << "Response: Unknown";

        qCWarning(client_tcp_transport)
            << "Unknown message type:"
            << static_cast<std::underlying_type_t<TransportStatus>>(status);
    }
}

void ClientTcpTransport::onSocketError([[maybe_unused]] QAbstractSocket::SocketError error)
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCWarning(client_tcp_transport)
        << "Error on socket:" << messageSocket->socket()->socketDescriptor();
    qCWarning(client_tcp_transport) << "Error:" << messageSocket->socket()->errorString();

    emit errorOccurred(messageSocket->socket()->errorString());
}
