#include "client.hpp"

Client::Client(const QString &connectionsFilePath, const QString &downloadsFilePath, QObject *parent)
    : QObject(parent)
    , m_clientHttpMessenger(m_directoryManager)
    , m_connectionManager(connectionsFilePath)
    , m_downloadManager(downloadsFilePath)

{
    m_clientMessenger = &m_clientHttpMessenger; // Temporarily
    m_connectionManager.readSavedConnections();
    signalSlotConnection();
    // connect(&m_timer, &QTimer::timeout, this, &ClientHttpMessenger::startConnectionVerification);
}

void Client::changeConnection(int index)
{
    m_connectionManager.setActiveConnection(index);
}

void Client::checkConnectionToServer()
{
    m_clientMessenger->checkConnectionToServer(m_connectionManager.getActiveConnection());
}

void Client::getDirectory(const QString &dirPath)
{
    m_clientMessenger->getDirectory(m_connectionManager.getActiveConnection(), dirPath);
}

void Client::signalSlotConnection()
{
    connect(m_clientMessenger,
            &ClientMessenger::statusCodeChanged,
            this,
            &Client::connectionStatusCodeChanged);

    connect(m_clientMessenger,
            &ClientMessenger::currentDirectoryChanged,
            this,
            &Client::onCurrentDirectoryChanged);
}

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_directoryManager.getActiveDirectory());
}

// void Client::getFile(const QString &path, const QString &savePath, const QString &saveName)
// {
//     m_currentClientNetworkProtocol->getFile(m_currentDirectory,
//                                       m_connectionManager.getActive().m_currentHostName,
//                                       m_downloadManager,
//                                       path,
//                                       savePath,
//                                       saveName);
// }

// void Client::startDownload(const QString &downloadID)
// {
//     m_currentClientNetworkProtocol->startDownload(downloadID);
// }

// void Client::stopDownload(const QString &downloadID)
// {
//     m_currentClientNetworkProtocol->stopDownload(downloadID);
// }
