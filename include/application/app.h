#ifndef APP_H
#define APP_H

#include <QObject>
#include "client.h"
#include "initialize.h"
// #include "server.h"

/**
 * @brief The App class
 * 
 * The class is responsible for initializing configuration files 
 * and managing the launch of the server and client parts.
 */
class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

private:
    Initialize m_initializer;
    Client m_client;
    // Server m_server;
};

#endif // APP_H
