#include "server.hpp"
#include "server_utils.hpp"

Q_LOGGING_CATEGORY(server, "server")

Server::Server(QObject *parent)
    : QObject(parent)
    , m_hostKey(createHostKey())
    , m_httpMessenger(m_hostKey)
    , m_tcpTransport()
{
    qCDebug(server) << "Server - created";
}

Server::~Server()
{
    stopServers();
    qCDebug(server) << "Server - destroyed";
}

void Server::setConfigPreferences(bool defaultConfigEnabled, bool secureConfigEnabled)
{
    qCDebug(server) << "Installing a new server configuration";

    m_defaultConfigEnabled = defaultConfigEnabled;
    m_secureConfigEnabled = secureConfigEnabled;
}

bool Server::startServers()
{
    qCDebug(server) << "Starting all servers";

    if (m_messengerEnabled) {
        m_httpMessenger.startAll();
    }
    if (m_transportEnabled) {
        m_tcpTransport.start();
    }

    return true;
}

bool Server::stopServers()
{
    qCDebug(server) << "Stopping all servers";

    m_httpMessenger.stopAll();
    m_tcpTransport.stop();
    return true;
}
