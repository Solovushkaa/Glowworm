#ifndef MESSAGESOCKET_HPP
#define MESSAGESOCKET_HPP

#include <QByteArray>
#include <QLoggingCategory>
#include <QObject>
#include <QSslSocket>

Q_DECLARE_LOGGING_CATEGORY(message_socket)

namespace constants {

inline constexpr quint16 kTransportHeaderSize = sizeof(quint32);

}

enum class TransportStatus : uint8_t {
    RequestFile,
    ResponseOk,
    ResponseError,
    FileChunk,
    TransferComplete
};

namespace message {

uint8_t toByteFromStatus(TransportStatus status);

}

class MessageSocket : public QObject
{
    Q_OBJECT

public:
    explicit MessageSocket(QObject *parent = nullptr);
    MessageSocket(QSslSocket *socket, QObject *parent = nullptr);
    ~MessageSocket();

    void connectToHost(const QString &host, const int port);

    void sendMessage(const QByteArray &data);
    QSslSocket *socket() const;

private:
    void connectSignals();

    void tryExtractMessages();

signals:
    void connected();
    void disconnected();
    void errorOccurred(QAbstractSocket::SocketError);
    void messageReceived(const QByteArray &message);
    void sslErrors(const QList<QSslError> &errors);
    void encrypted();

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSslError(const QList<QSslError> &errors);

private:
    QSslSocket *m_socket;
    QByteArray m_buffer;
};

#endif // MESSAGESOCKET_HPP
