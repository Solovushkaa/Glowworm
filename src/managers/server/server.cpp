#include "server.hpp"
#include "server_utils.hpp"

Server::Server(QObject *parent)
    : QObject(parent)
    , m_hostKey(createHostKey())
    , m_serverHttpMessenger(m_hostKey)
{
    qDebug() << "Creating Server";
    m_serverMessenger = &m_serverHttpMessenger; // Temporarily
}

bool Server::startServer()
{
    bool useDefaultConfig = true; // Temporarily
    bool useSecureConfig = false; // Temporarily
    return m_serverMessenger->start(useDefaultConfig, useSecureConfig);
}

bool Server::stopServer()
{
    bool stopDefaultConfig = true; // Temporarily
    bool stopSecureConfig = false; // Temporarily
    m_serverMessenger->stop(stopDefaultConfig, stopSecureConfig);

    return true;
}
