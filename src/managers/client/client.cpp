#include "client.hpp"
#include "constants.hpp"

Q_LOGGING_CATEGORY(client, "client")

Client::Client(const QString &connectionsFilePath, const QString &downloadsFilePath, QObject *parent)
    : QObject(parent)
    , m_httpMessenger(m_connectionManager, m_directoryManager)
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

void Client::getFile(int fileIndex, const QString &saveName, const QString &savePath)
{
    qCDebug(client) << "Getting file:"
                    << m_directoryManager.getActiveDirectory()[fileIndex]->m_path;

    if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
        return;
    }

    const QString downloadID = generateDownloadID(fileIndex);

    auto activeConnection = getActiveConnection();
    auto fileInfo = m_directoryManager.getActiveDirectory()[fileIndex];
    m_downloadManager.addDownload(downloadID,
                                  activeConnection->m_url,
                                  activeConnection->m_hostKey,
                                  fileInfo->m_name,
                                  fileInfo->m_path,
                                  saveName,
                                  savePath,
                                  fileInfo->m_size,
                                  0,
                                  fileInfo->m_created,
                                  fileInfo->m_modified,
                                  fileInfo->m_accessed,
                                  DownloadInfo::DownloadState::Wait);

    connect(&m_dataExchanger, &ClientTcpTransport::fileReceived, this, &Client::fileReceived);

    m_dataExchanger.getFile(getActiveConnection()->m_url,
                            m_downloadManager.getDownloadInfoDict()[downloadID]);
}

void Client::startDownload(int /*downloadIndex*/)
{
    if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
        return;
    }
    // m_dataExchangers[downloadID].start(/**/);
}

void Client::stopDownload(int /*downloadIndex*/)
{
    // m_dataExchangers[downloadID].stop(/**/);
}

ConnectionInfo *Client::getActiveConnection()
{
    qCDebug(client) << "Getting an active connection";

    return m_connectionManager.getActiveConnection();
}

void Client::setConnectionPreferences()
{
    qCDebug(client) << "Changing connection preferences";

    connectSignals();

    qCInfo(client) << "New connection settings have been applied.";
}

QString Client::generateDownloadID(int fileIndex)
{
    QString downloadID
        = QCryptographicHash::hash((m_directoryManager.getActiveDirectory()[fileIndex]->m_path
                                    + getActiveConnection()->m_hostKey)
                                       .toUtf8(),
                                   QCryptographicHash::Sha256)
              .toHex()
              .left(constants::kDownloadIDLength);

    return downloadID;
}

void Client::connectSignals()
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

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_directoryManager.getActiveDirectory());
}
