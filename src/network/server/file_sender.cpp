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
        qCWarning(server_file_sender) << "Unable to open file" << m_file.fileName();
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

    // Однократное подключение сигнала bytesWritten (с предварительным отключением на всякий случай)
    QTcpSocket *socket = m_client->socket();
    disconnect(socket, &QTcpSocket::bytesWritten, this, &FileSender::onBytesWritten);
    connect(socket, &QTcpSocket::bytesWritten, this, &FileSender::onBytesWritten);

    // Запускаем первый цикл отправки (через event loop для неблокирующего старта)
    QMetaObject::invokeMethod(this, "sendNextChunk", Qt::QueuedConnection);
}

void FileSender::sendNextChunk()
{
    qCDebug(server_file_sender) << "Sending the next chunk for socket:"
                                << m_client->socket()->socketDescriptor();

    if (!m_transferActive)
        return;

    QTcpSocket *socket = m_client->socket();

    // Пишем чанки, пока есть данные и буфер не достиг верхней границы
    while (!m_file.atEnd()) {
        // Верхняя граница буфера (можно вынести в константу, например 256 КБ)
        static const qint64 HighWaterMark = 2 * constants::kTransportChunkSize;
        if (socket->bytesToWrite() > HighWaterMark) {
            // Буфер почти полон – прекращаем запись, продолжим в onBytesWritten
            return;
        }

        QByteArray chunk = m_file.read(constants::kTransportChunkSize);
        QByteArray packet;
        packet.append(message::toByteFromStatus(TransportStatus::FileChunk));
        packet.append(chunk);
        m_client->sendMessage(packet);
    }

    // Файл прочитан полностью. Если буфер ещё не пуст, дождёмся сигнала bytesWritten
    // (окончание передачи обработаем в onBytesWritten)
}

void FileSender::onBytesWritten(qint64 bytes)
{
    qCDebug(server_file_sender) << "Bytes written:" << bytes;

    QTcpSocket *socket = m_client->socket();
    const qint64 buffered = socket->bytesToWrite();

    // Нижняя граница – можно снова досылать данные
    static const qint64 LowWaterMark = constants::kTransportChunkSize;

    if (m_transferActive && buffered <= LowWaterMark && !m_file.atEnd()) {
        // Буфер опустел достаточно – продолжаем отправку
        sendNextChunk();
    } else if (m_transferActive && m_file.atEnd() && buffered == 0) {
        // Всё отправлено, завершаем передачу
        QByteArray complete;
        complete.append(message::toByteFromStatus(TransportStatus::TransferComplete));
        complete.append(m_downloadID.toUtf8());
        m_client->sendMessage(complete);

        m_transferActive = false;
        emit finished();
    }
}
