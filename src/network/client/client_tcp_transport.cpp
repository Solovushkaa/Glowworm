#include "client_tcp_transport.hpp"
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

void ClientTcpTransport::getFile(const QUrl &url, DownloadInfo *downloadInfo)
{
    qCDebug(client_tcp_transport) << "Getting file:" << downloadInfo->m_path
                                  << "From host:" << url.host();

    const QString &downloadID = downloadInfo->m_downloadID;

    m_sockets[downloadID] = new ClientMessageSocket(this);
    m_sockets[downloadID]->setDownloadInfo(downloadInfo);
    connectSignals(m_sockets[downloadID]);

    startNewDownload(url, downloadID);

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

void ClientTcpTransport::startNewDownload(const QUrl &url, const QString &downloadID)
{
    qCDebug(client_tcp_transport) << "Starting new download on host:" << url.host();
    m_sockets[downloadID]->connectToHost(url.host(), 6821 /*url.port()*/);
}

void ClientTcpTransport::requestFile(DownloadInfo *downloadInfo)
{
    qCDebug(client_tcp_transport) << "File request:" << downloadInfo->m_path;

    auto socket = m_sockets[downloadInfo->m_downloadID];

    if (socket->socket()->state() != QTcpSocket::ConnectedState) {
        emit errorOccurred("Not connected");
        return;
    }

    QByteArray request;
    request.append(message::toByteFromStatus(TransportStatus::RequestFile));
    request.append(downloadInfo->m_downloadID.toUtf8());
    request.append(downloadInfo->m_path.toUtf8());

    socket->sendMessage(request);

    qCInfo(client_tcp_transport) << "File request sent";
}

void ClientTcpTransport::onConnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_transport) << "Connection established via socket descriptor:"
                                 << messageSocket->socket()->socketDescriptor();

    requestFile(messageSocket->getDownloadInfo());

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
    qCInfo(client_tcp_transport) << "Received message";
    if (message.isEmpty())
        return;

    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    qCInfo(client_tcp_transport) << "Message received on socket:"
                                 << messageSocket->socket()->socketDescriptor();

    auto downloadInfo = messageSocket->getDownloadInfo();
    auto downloadID = downloadInfo->m_downloadID;

    TransportStatus status = static_cast<TransportStatus>(message.at(0));
    qCDebug(client_tcp_transport) << "Message status" << static_cast<int>(status);
    QByteArray payload = message.mid(1);

    switch (status) {
    case TransportStatus::ResponseOk: {
        qCInfo(client_tcp_transport) << "Response: Ok";
        bool statusOk;
        downloadInfo->m_size = payload.toLongLong(&statusOk);
        if (!statusOk || downloadInfo->m_size < 0) {
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            downloadInfo->m_size = 0;
            emit errorOccurred("Invalid file size received");
            return;
        }

        m_outputFiles.insert(downloadID, new QFile(downloadInfo->m_savePath, this));
        if (!m_outputFiles[downloadID]->open(QIODevice::WriteOnly)) {
            downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);

            emit errorOccurred("Cannot save file: " + m_outputFiles[downloadID]->errorString());
            return;
        }

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Active);

        qCInfo(client_tcp_transport) << "Receiving file of size" << downloadInfo->m_size << "bytes";

        break;
    }
    case TransportStatus::ResponseError: {
        qCInfo(client_tcp_transport) << "Response: Error";
        qCWarning(client_tcp_transport) << QString::fromUtf8(payload);

        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred(QString::fromUtf8(payload));

        break;
    }
    case TransportStatus::FileChunk: {
        qCInfo(client_tcp_transport) << "Response: FileChunk";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active
            || !m_outputFiles[downloadID]->isOpen()) {
            emit errorOccurred("Unexpected file chunk");
            return;
        }
        m_outputFiles[downloadID]->write(payload);

        break;
    }
    case TransportStatus::TransferComplete: {
        qCInfo(client_tcp_transport) << "Response: TransferComplete";

        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active) {
            emit errorOccurred("Unexpected transfer complete");
            return;
        }

        if (m_outputFiles[downloadID]->size() != downloadInfo->m_size) {
            emit errorOccurred("File size mismatch");
            return;
        }

        m_outputFiles[downloadID]->deleteLater();
        m_outputFiles.remove(downloadID);
        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Finish);

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
