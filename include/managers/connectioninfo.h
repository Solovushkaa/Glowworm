#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QObject>

class ConnectionInfo : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionInfo(QObject *parent = nullptr);

    /* Members */
public:
    QString m_url;
    QString m_name;
    QString m_protocol;

signals:
    void connectionInfoChanged();
};

#endif // CONNECTIONINFO_H
