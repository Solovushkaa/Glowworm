#include "message_socket.hpp"
#include <QDataStream>

Q_LOGGING_CATEGORY(message_socket, "utils.socket.message")

MessageSocket::MessageSocket(QObject *parent)
    : QObject(parent)
    , m_socket(new QSslSocket(this))
{
    connectSignals();

    qCDebug(message_socket) << "MessageSocket" << m_socket->socketDescriptor() << "- created";
}

MessageSocket::MessageSocket(QSslSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
    m_socket->setParent(this);

    connectSignals();

    qCDebug(message_socket) << "MessageSocket" << m_socket->socketDescriptor() << "- created";
}

MessageSocket::~MessageSocket()
{
    qCDebug(message_socket) << "MessageSocket" << m_socket->socketDescriptor() << "- destroyed";
}

void MessageSocket::connectToHost(const QString &host, const int port)
{
    m_socket->connectToHostEncrypted(host, port);
}

void MessageSocket::sendMessage(const QByteArray &data)
{
    qCDebug(message_socket) << "Sending message";

    if (!m_socket || m_socket->state() != QSslSocket::ConnectedState) {
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_9);

    out << static_cast<quint32>(data.size());
    block.append(data);
    m_socket->write(block);
    // m_socket->flush();

    qCDebug(message_socket) << "Another block of data has been sent:" << block.size() << "B";
}

QSslSocket *MessageSocket::socket() const
{
    return m_socket;
}

void MessageSocket::connectSignals()
{
    connect(m_socket, &QSslSocket::readyRead, this, &MessageSocket::onReadyRead);
    connect(m_socket, &QSslSocket::connected, this, &MessageSocket::connected);
    connect(m_socket, &QSslSocket::disconnected, this, &MessageSocket::onDisconnected);
    connect(m_socket, &QSslSocket::disconnected, this, &MessageSocket::disconnected);
    connect(m_socket, &QSslSocket::errorOccurred, this, &MessageSocket::errorOccurred);
    connect(m_socket, &QSslSocket::sslErrors, this, &MessageSocket::onSslError);
    connect(m_socket, &QSslSocket::encrypted, this, &MessageSocket::encrypted);
}

void MessageSocket::tryExtractMessages()
{
    qCDebug(message_socket) << "Trying to extract a message";

    while (m_buffer.size() >= constants::kTransportHeaderSize) {
        QDataStream stream(m_buffer);
        stream.setVersion(QDataStream::Qt_6_9);
        quint32 messageSize = 0;
        stream >> messageSize;
        if (m_buffer.size() < constants::kTransportHeaderSize + static_cast<int>(messageSize)) {
            break;
        }

        QByteArray message = m_buffer.mid(constants::kTransportHeaderSize, messageSize);
        m_buffer.remove(0, constants::kTransportHeaderSize + messageSize);

        qCDebug(message_socket) << "Message received";
        emit messageReceived(message);
    }
}

void MessageSocket::onReadyRead()
{
    qCDebug(message_socket) << "Socket" << m_socket->socketDescriptor() << "is ready for reading";

    m_buffer.append(m_socket->readAll());
    tryExtractMessages();
}

void MessageSocket::onDisconnected()
{
    qCDebug(message_socket) << "Socket" << m_socket->socketDescriptor() << "disconnected";

    emit disconnected();
}

void MessageSocket::onSslError(const QList<QSslError> &errors)
{
    emit sslErrors(errors);
}

namespace message {

uint8_t toByteFromStatus(TransportStatus status)
{
    return static_cast<std::underlying_type_t<TransportStatus>>(status);
}

} // namespace message
