#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
    , m_connManager()
    , m_downloadManager()
    , m_localClient()
{
    // connect(&m_timer, &QTimer::timeout, this, &HttpClient::startConnectionVerification);
}

void Client::checkConnectionToServer()
{
    m_currentNetworkInterface->checkConnectionToServer(m_connManager.getActive().m_url);
}

void Client::getDirectoryList(const QString &path, const QString &url, const QString &userID)
{
    m_currentNetworkInterface->getDirectoryList(path, url, userID);
}

void Client::getFile(QList<QVariantHash> &currentDirectory,
                     QHash<QString, DownloadInfo> &downloadInfoDict,
                     const QString &path,
                     const QString &savePath,
                     const QString &saveName)
{
    m_currentNetworkInterface->getFile(currentDirectory,
                                       m_connManager.getActive().m_currentHostName,
                                       downloadInfoDict,
                                       path,
                                       savePath,
                                       saveName);
}

void Client::startDownload(const QString &downloadID)
{
    m_currentNetworkInterface->startDownload(downloadID);
}

void Client::stopDownload(const QString &downloadID)
{
    m_currentNetworkInterface->stopDownload(downloadID);
}
