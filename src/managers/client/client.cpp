#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
    , m_connectionManager()
    , m_downloadManager()
{
    // connect(&m_timer, &QTimer::timeout, this, &DirectClient::startConnectionVerification);
}

void Client::changeConnection(int index)
{
    m_connectionManager.setActive(index);
}

void Client::checkConnectionToServer()
{
    m_currentClientNetworkProtocol->checkConnectionToServer(m_connectionManager.getActive().m_url);
}

void Client::getDirectoryList(const QString &path, const QString &url, const QString &userID)
{
    m_currentClientNetworkProtocol->getDirectoryList(path, url, userID);
}

void Client::getFile(const QString &path, const QString &savePath, const QString &saveName)
{
    m_currentClientNetworkProtocol->getFile(m_currentDirectory,
                                      m_connectionManager.getActive().m_currentHostName,
                                      m_downloadManager,
                                      path,
                                      savePath,
                                      saveName);
}

void Client::startDownload(const QString &downloadID)
{
    m_currentClientNetworkProtocol->startDownload(downloadID);
}

void Client::stopDownload(const QString &downloadID)
{
    m_currentClientNetworkProtocol->stopDownload(downloadID);
}
