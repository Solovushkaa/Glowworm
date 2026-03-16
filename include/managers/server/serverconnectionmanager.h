#ifndef SERVERCONNECTIONMANAGER_H
#define SERVERCONNECTIONMANAGER_H

#include <QObject>

class ServerConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerConnectionManager(QObject *parent = nullptr);

signals:
};

#endif // SERVERCONNECTIONMANAGER_H
