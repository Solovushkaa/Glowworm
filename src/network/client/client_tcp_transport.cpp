#include "client_tcp_transport.hpp"
#include <QDataStream>
#include <QDir>
#include <QFileInfo>

ClientTcpTransport::ClientTcpTransport(QObject *parent)
    : QObject(parent)
{
    // connect(this, &ClientTcpTransport::fileReceived, this, &ClientTcpTransport::onFileReceived);
}

ClientTcpTransport::~ClientTcpTransport() {}

void ClientTcpTransport::getFile(const QUrl &url, DownloadInfo *downloadInfo)
{
    qCritical() << "Get File";

    const QString &downloadID = downloadInfo->m_downloadID;

    m_sockets[downloadID] = new ClientMessageSocket(this);
    m_sockets[downloadID]->setDownloadInfo(downloadInfo);
    connectSignals(m_sockets[downloadID]);

    startNewDownload(url, downloadID);
}

void ClientTcpTransport::connectSignals(ClientMessageSocket *messenger)
{
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
    qCritical() << "Starting new download(host):" << url.host();
    m_sockets[downloadID]->connectToHost(url.host(), 6821 /*url.port()*/);
}

void ClientTcpTransport::requestFile(DownloadInfo *downloadInfo)
{
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
}

void ClientTcpTransport::onConnected()
{
    qInfo() << "Connected to server";

    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());
    requestFile(messageSocket->getDownloadInfo());

    emit connected();
}

void ClientTcpTransport::onDisconnected()
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    const QString &downloadID = messageSocket->getDownloadInfo()->m_downloadID;
    if (m_outputFiles[downloadID]->isOpen()) {
        m_outputFiles.remove(downloadID);
    }

    emit disconnected();
}

void ClientTcpTransport::onMessageReceived(const QByteArray &message)
{
    qInfo() << "onMessageReceived";
    if (message.isEmpty())
        return;

    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());
    auto downloadInfo = messageSocket->getDownloadInfo();
    auto downloadID = downloadInfo->m_downloadID;

    TransportStatus status = static_cast<TransportStatus>(message.at(0));
    qCritical() << static_cast<int>(status);
    QByteArray payload = message.mid(1);

    switch (status) {
    case TransportStatus::ResponseOk: {
        qDebug() << "Response Ok";
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

        qInfo() << "Receiving file of size" << downloadInfo->m_size << "bytes";
        break;
    }
    case TransportStatus::ResponseError: {
        qCritical() << QString::fromUtf8(payload);
        downloadInfo->setDownloadState(DownloadInfo::DownloadState::Error);
        emit errorOccurred(QString::fromUtf8(payload));
        break;
    }
    case TransportStatus::FileChunk: {
        if (downloadInfo->m_downloadState != DownloadInfo::DownloadState::Active
            || !m_outputFiles[downloadID]->isOpen()) {
            emit errorOccurred("Unexpected file chunk");
            return;
        }
        m_outputFiles[downloadID]->write(payload);
        break;
    }
    case TransportStatus::TransferComplete: {
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

        emit fileReceived(downloadInfo->m_saveName);
        break;
    }
    default:
        qWarning() << "Unknown message type:"
                   << static_cast<std::underlying_type_t<TransportStatus>>(status);
    }
}

void ClientTcpTransport::onSocketError([[maybe_unused]] QAbstractSocket::SocketError error)
{
    auto messageSocket = qobject_cast<ClientMessageSocket *>(sender());

    emit errorOccurred(messageSocket->socket()->errorString());
}
