#ifndef SERVERSAVEDCONNECTIONMANAGER_H
#define SERVERSAVEDCONNECTIONMANAGER_H

#include <QObject>

class ServerSavedConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerSavedConnectionManager(QObject *parent = nullptr);

signals:
};

#endif // SERVERSAVEDCONNECTIONMANAGER_H
