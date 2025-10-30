#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QObject>

class ConnectionInfo : public QObject
{
    Q_OBJECT

    /* Constructors */
public:
    explicit ConnectionInfo(QObject *parent = nullptr);

    ConnectionInfo(const ConnectionInfo &connInfo) { *this = connInfo; }
    ConnectionInfo &operator=(const ConnectionInfo &connInfo);
    ConnectionInfo(ConnectionInfo &&connInfo) { *this = std::move(connInfo); }
    ConnectionInfo &operator=(ConnectionInfo &&connInfo);

    /* Members */
public:
    QString m_url;
    QString m_name;
    QString m_protocol;
    QString m_ip;
    qint16 m_port;

    /* Signals */
signals:
    void connectionInfoChanged();
};

#endif // CONNECTIONINFO_H
