#include "client.hpp"

Q_LOGGING_CATEGORY(client, "client")

Client::Client(const QString &connectionsFilePath, const QString &downloadsFilePath, QObject *parent)
    : QObject(parent)
    , m_httpMessenger(m_connectionManager, m_directoryManager)
    , m_tcpTransport()
    , m_connectionManager(connectionsFilePath)
    , m_downloadManager(downloadsFilePath)

{
    // connect(&m_timer, &QTimer::timeout, this, &ClientHttpMessenger::startConnectionVerification);
    m_connectionManager.readSavedConnections();
    m_downloadManager.readUnfinishedDownloads();

    qCDebug(client) << "Client - created";
}

Client::~Client()
{
    qCDebug(client) << "Client - destroyed";
}

void Client::setActiveConnection(int index)
{
    qCDebug(client) << "Changing the active connection";

    m_connectionManager.setActiveConnection(index);
    setConnectionPreferences();

    qCInfo(client) << "Active connection changed";
}

void Client::checkConnectionToServer()
{
    qCDebug(client) << "Connection check request";

    m_httpMessenger.checkConnectionToServer(getActiveConnection());
}

void Client::getDirectory(const QString &dirPath)
{
    qCDebug(client) << "Directory request";

    m_httpMessenger.getDirectory(getActiveConnection(), dirPath);
}

ConnectionInfo *Client::getActiveConnection()
{
    qCDebug(client) << "Getting an active connection";

    return m_connectionManager.getActiveConnection();
}

void Client::setConnectionPreferences()
{
    qCDebug(client) << "Changing connection preferences";

    // ConnectionInfo *connectionInfo = getActiveConnection();
    // if (connectionInfo->m_transport != ConnectionInfo::Transport::TCP) {
    //     // set TCP transport;
    // } else if (connectionInfo->m_transport != ConnectionInfo::Transport::UDP) {
    //     // set UDP transport;
    // }

    signalSlotConnection();

    qCInfo(client) << "New connection settings have been applied.";
}

void Client::signalSlotConnection()
{
    qCDebug(client) << "Connecting slots and signals";

    connect(&m_httpMessenger,
            &ClientHttpMessenger::statusCodeChanged,
            this,
            &Client::connectionStatusCodeChanged);

    connect(&m_httpMessenger,
            &ClientHttpMessenger::currentDirectoryChanged,
            this,
            &Client::onCurrentDirectoryChanged);
}

void Client::getFile(int fileIndex)
{
    qDebug() << fileIndex;
}

// void Client::startDownload(const QString &downloadID)
// {
//     m_currentClientNetworkProtocol->startDownload(downloadID);
// }

// void Client::stopDownload(const QString &downloadID)
// {
//     m_currentClientNetworkProtocol->stopDownload(downloadID);
// }

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_directoryManager.getActiveDirectory());
}
