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

    if (!m_webSocketMessengers.contains(getActiveConnection()->m_name)) {
        auto messenger
            = new ClientWebSocketMessenger(getActiveConnection(),
                                           getActiveConnection()->m_remoteUserUuid.right(62).toUtf8(),
                                           m_directoryManager,
                                           this);
        m_webSocketMessengers.insert(getActiveConnection()->m_name, messenger);
    }

    setConnectionPreferences();

    qCInfo(client) << "Active connection changed";
}

void Client::getDirectory(const QString &dirPath)
{
    qCDebug(client) << "Directory request";

    if (m_webSocketMessengers.contains(getActiveConnection()->m_name)) {
        m_webSocketMessengers[getActiveConnection()->m_name]->getDirectory(dirPath);
    } else {
        qCWarning(client) << "Directory create request error: Connection doesn't exist";
    }
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

    connect(&m_dataExchanger, &ClientTcpTransport::fileReceived, this, &Client::fileReceived);

    m_dataExchanger.getFile(getActiveConnection()->m_address,
                            getActiveConnection()->m_transportPort,
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

void Client::connectToRelayServer()
{
    qCDebug(client) << "Connection check realy server request";

    // m_webSocketMessengers[getActiveConnection()->m_name].connectToRelayServer(getActiveConnection());
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

    connect(&m_dataExchanger, &ClientTcpTransport::fileReceived, this, &Client::fileReceived);

    m_dataExchanger.getFileFromRelay(getActiveConnection()->m_address,
                                     getActiveConnection()->m_transportPort,
                                     userName,
                                     m_downloadManager.getDownloadInfoDict()[downloadID]);
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

    connect(m_webSocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::statusCodeChanged,
            this,
            &Client::connectionStatusCodeChanged);

    connect(m_webSocketMessengers[getActiveConnection()->m_name],
            &ClientWebSocketMessenger::currentDirectoryChanged,
            this,
            &Client::onCurrentDirectoryChanged);
}

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_directoryManager.getActiveDirectory());
}
