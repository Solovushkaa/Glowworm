#ifndef FILESENDER_HPP
#define FILESENDER_HPP

#include <QFile>
#include <QObject>
#include "message_socket.hpp"

Q_DECLARE_LOGGING_CATEGORY(server_file_sender)

namespace constants {

inline constexpr qint32 kTransportChunkSize = 65536;
inline constexpr qint32 kTransportMaxChunkSize = kTransportChunkSize * 2;

} // namespace constants

class FileSender : public QObject
{
    Q_OBJECT
public:
    explicit FileSender(MessageSocket *client,
                        const QString &downloadID,
                        const QString &filePath,
                        QObject *parent = nullptr);
    ~FileSender();

    void start();

signals:
    void finished();
    void errorOccurred(const QString &message);

private slots:
    void onBytesWritten(qint64 bytes);
    void sendNextChunk();

private:
    MessageSocket *m_client;

    QString m_downloadID;
    QFile m_file;

    qint64 m_fileSize{0};
    bool m_transferActive{false};
};

#endif // FILESENDER_HPP
