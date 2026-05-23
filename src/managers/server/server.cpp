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
    stopDefaultServer();
    qCDebug(server) << "Server - destroyed";
}

bool Server::startDefaultServer()
{
    qCDebug(server) << "Starting default server";

    bool started = true;
    started &= m_httpMessenger.startDefault();
    started &= m_tcpTransport.start();

    if (started) {
        qCInfo(server) << "Default server started";
    } else {
        qCCritical(server) << "Error starting default server";
    }

    return started;
}

void Server::stopDefaultServer()
{
    qCDebug(server) << "Stopping default server";

    m_httpMessenger.stopDefault();
    m_tcpTransport.stop();

    qCInfo(server) << "Default server stopped";
}
