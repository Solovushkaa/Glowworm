#ifndef APP_H
#define APP_H

#include <QObject>
#include "client.h"
#include "initialize.h"
#include "server.h"

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

private:
    Initialize m_initializer;
    Client m_client;
    Server m_server;
};

#endif // APP_H
