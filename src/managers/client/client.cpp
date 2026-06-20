#include "client.hpp"
#include "constants.hpp"

Q_LOGGING_CATEGORY(client, "client")

Client::Client(const QString &connectionsFilePath, const QString &downloadsFilePath, QObject *parent)
    : QObject(parent)
    , m_connectionManager(connectionsFilePath)
    , m_downloadManager(downloadsFilePath)

{
    m_connectionManager.readSavedConnections();
    m_downloadManager.readUnfinishedDownloads();

    // connectSignals();

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

    if (!m_websocketMessengers.contains(getActiveConnection()->m_name)) {
        connectToServer();

        setConnectionPreferences();
    }

    qCInfo(client) << "Active connection changed";
}

void Client::connectToServer()
{
    if (!m_websocketMessengers.contains(getActiveConnection()->m_name)) {
        m_websocketMessengers[getActiveConnection()->m_name]
            = new ClientWebSocketMessenger(getActiveConnection(),
                                           getActiveConnection()->m_remoteUserUuid.toUtf8(),
                                           m_directoryManager,
                                           this);

        setConnectionPreferences();
    }

    m_websocketMessengers[getActiveConnection()->m_name]->connectToServer();
}

void Client::disconnectFromServer()
{
    if (m_websocketMessengers.contains(getActiveConnection()->m_name)) {
        m_websocketMessengers[getActiveConnection()->m_name]->disconnectFromServer();
    }
}

void Client::getDirectory(const QString &dirPath)
{
    qCDebug(client) << "Directory request:" << getActiveConnection()->m_webDavConnection; // TODO:

    if (m_websocketMessengers.contains(getActiveConnection()->m_name)) {
        m_websocketMessengers[getActiveConnection()->m_name]->getDirectory(dirPath);
    } else {
        qCWarning(client) << "Directory create request error: Connection doesn't exist";
    }
}

void Client::getWebDavDirectory(const QString &dirPath)
{
    qCDebug(client) << "Directory WebDAV request";

    if (!m_webDavMessengers.contains(getActiveConnection()->m_name)) {
        m_webDavMessengers[getActiveConnection()->m_name]
            = new ClientWebDAV(getActiveConnection()->m_address,
                               getActiveConnection()->m_messengerPort,
                               getActiveConnection()->m_webDavUsername,
                               getActiveConnection()->m_webDavPassword,
                               m_directoryManager);
    }

    m_webDavMessengers[getActiveConnection()->m_name]->getDirectory(dirPath);
}

void Client::getFile(int fileIndex, const QString &saveName, const QString &savePath)
{
    getFileRange(fileIndex, saveName, savePath, 0);
}

void Client::getFileRange(int fileIndex,
                          const QString &saveName,
                          const QString &savePath,
                          qint64 begin)
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
                                  activeConnection->m_address,
                                  activeConnection->m_hostKey,
                                  fileInfo->m_name,
                                  fileInfo->m_path,
                                  saveName,
                                  savePath,
                                  fileInfo->m_size,
                                  begin,
                                  fileInfo->m_created,
                                  fileInfo->m_modified,
                                  fileInfo->m_accessed,
                                  DownloadInfo::DownloadState::Wait);

    bool isWebDAV = false;
    if (isWebDAV) {
        // m_tcpTransports[downloadID] = new ClientWebDav;
    } else {
        m_tcpTransports[downloadID] = new ClientTcpTlsTransport(savePath, this);
    }

    connect(m_tcpTransports[downloadID],
            &ClientTcpTlsTransport::fileReceived,
            this,
            &Client::fileReceived);

    m_tcpTransports[downloadID]->getFile(getActiveConnection()->m_address,
                                         getActiveConnection()->m_transportPort,
                                         m_downloadManager.getDownloadInfoDict()[downloadID]);
}

void Client::startDownload(int /*downloadIndex*/)
{
    if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
        return;
    }

    // m_tcpTransports[downloadID].start(/**/);
}

void Client::stopDownload(int /*downloadIndex*/)
{
    // m_tcpTransports[downloadID].stop(/**/);
}

void Client::connectToRelayServer()
{
    qCDebug(client) << "Connection check realy server request";

    // m_websocketMessengers[getActiveConnection()->m_name].connectToRelayServer(getActiveConnection());
}

void Client::getFileFromRelayServer(int fileIndex,
                                    const QString &userName,
                                    const QString &saveName,
                                    const QString &savePath)
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
                                  activeConnection->m_address,
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

    connect(m_tcpTransports[downloadID],
            &ClientTcpTlsTransport::fileReceived,
            this,
            &Client::fileReceived);

    m_tcpTransports[downloadID]
        ->getFileFromRelay(getActiveConnection()->m_address,
                           getActiveConnection()->m_transportPort,
                           userName,
                           m_downloadManager.getDownloadInfoDict()[downloadID]);
}

void Client::updateConnection(ConnectionInfo *connectionInfo)
{
    m_connectionManager.updateConnection(connectionInfo);
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

    connect(m_websocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::statusCodeChanged,
            this,
            &Client::connectionStatusCodeChanged);

    connect(m_websocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::currentDirectoryChanged,
            this,
            &Client::onCurrentDirectoryChanged);

    connect(m_websocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::connected,
            this,
            &Client::updateConnection);
    connect(m_websocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::disconnected,
            this,
            &Client::updateConnection);
}

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_directoryManager.getActiveDirectory());
}
