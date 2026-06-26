#ifndef APP_HPP
#define APP_HPP

#include <QObject>
#include "client.hpp"
#include "initialize.hpp"
#include "server.hpp"

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
    void registrateQmlTypes();
    void setGraphicPreferences();

private:
    Initialize m_initializer;

    Client m_client;
    Server m_server;
};

#endif // APP_HPP
