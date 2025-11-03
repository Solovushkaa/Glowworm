#ifndef LOCALCLIENT_H
#define LOCALCLIENT_H

#include <QObject>

class LocalClient : public QObject
{
    Q_OBJECT
public:
    explicit LocalClient(QObject *parent = nullptr);

signals:
};

#endif // LOCALCLIENT_H
