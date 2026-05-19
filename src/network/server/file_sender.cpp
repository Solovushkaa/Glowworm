#include "file_sender.hpp"
#include <QMetaObject>

Q_LOGGING_CATEGORY(server_file_sender, "server.file.sender")

FileSender::FileSender(MessageSocket *client,
                       const QString &downloadID,
                       const QString &filePath,
                       QObject *parent)
    : QObject(parent)
    , m_client(client)
    , m_downloadID(downloadID)
    , m_file(filePath)
{
    qCDebug(server_file_sender) << "FileSender - created";
}

FileSender::~FileSender()
{
    qCDebug(server_file_sender) << "FileSender - destroyed";
}

void FileSender::start()
{
    qCDebug(server_file_sender) << "FileSender - started";

    if (!m_file.open(QIODevice::ReadOnly)) {
        QByteArray response;
        response.append(message::toByteFromStatus(TransportStatus::ResponseError));
        response.append(m_file.errorString().toUtf8());

        m_client->sendMessage(response);

        emit errorOccurred(m_file.errorString());
        emit finished();

        return;
    }

    m_fileSize = m_file.size();

    QByteArray response;
    response.append(message::toByteFromStatus(TransportStatus::ResponseOk));
    response.append(QByteArray::number(m_fileSize));
    m_client->sendMessage(response);

    m_transferActive = true;

    QMetaObject::invokeMethod(this, "sendNextChunk", Qt::QueuedConnection);
}

void FileSender::sendNextChunk()
{
    qCDebug(server_file_sender) << "Sending the next chunk for socket:"
                                << m_client->socket()->socketDescriptor();
    if (!m_transferActive)
        return;

    if (m_file.atEnd()) {
        QByteArray complete;
        complete.append(message::toByteFromStatus(TransportStatus::TransferComplete));
        complete.append(m_downloadID.toUtf8());
        m_client->sendMessage(complete);
        m_transferActive = false;
        emit finished();
        return;
    }

    QByteArray chunk = m_file.read(constants::kTransportChunkSize);
    QByteArray packet;
    packet.append(message::toByteFromStatus(TransportStatus::FileChunk));
    packet.append(chunk);
    m_client->sendMessage(packet);

    QTcpSocket *socket = m_client->socket();
    if (socket->bytesToWrite() > constants::kTransportChunkSize) {
        connect(socket, &QTcpSocket::bytesWritten, this, &FileSender::onBytesWritten);
    } else {
        QMetaObject::invokeMethod(this, "sendNextChunk", Qt::QueuedConnection);
    }
}

void FileSender::onBytesWritten(qint64 bytes)
{
    qCDebug(server_file_sender) << "Bytes written:" << bytes;

    QTcpSocket *socket = m_client->socket();
    if (socket->bytesToWrite() <= constants::kTransportChunkSize && m_transferActive) {
        disconnect(socket, &QTcpSocket::bytesWritten, this, &FileSender::onBytesWritten);
        sendNextChunk();
    }
}
