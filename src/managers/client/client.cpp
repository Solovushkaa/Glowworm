#include "client.hpp"
#include "constants.hpp"
#include "json_utils.hpp"
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(client, "client")

Client::Client(const QString &connectionsFilePath, const QString &downloadsFilePath, QObject *parent)
    : QObject(parent)
    , m_connectionManager(connectionsFilePath)
    , m_downloadManager(downloadsFilePath)

{
    m_connectionManager.readSavedConnections();
    m_downloadManager.readUnfinishedDownloads();

    connect(&m_connectionManager,
            &ClientConnectionManager::activeConnectionChanged,
            this,
            &Client::onActiveConnectionChanged);

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
    }

    qCInfo(client) << "Active connection changed";
}

void Client::connectToServer()
{
    if (getActiveConnection() != nullptr && !getActiveConnection()->m_webDavConnection) {
        if (!m_websocketMessengers.contains(getActiveConnection()->m_name)) {
            m_websocketMessengers[getActiveConnection()->m_name]
                = new ClientWebSocketMessenger(getActiveConnection(),
                                               getActiveConnection()->m_remoteUserUuid.toUtf8(),
                                               m_networkDirectoryManager,
                                               this);

            setConnectionPreferences();
        }

        m_websocketMessengers[getActiveConnection()->m_name]->connectToServer();

    } else if (getActiveConnection() != nullptr) {
        if (!m_webDavMessengers.contains(getActiveConnection()->m_name)) {
            m_webDavMessengers[getActiveConnection()->m_name]
                = new ClientWebDAV(getActiveConnection(), m_networkDirectoryManager);

            setConnectionPreferences();
        }

        m_webDavMessengers[getActiveConnection()->m_name]->connectToServer();
    }
}

void Client::disconnectFromServer()
{
    if (getActiveConnection() != nullptr && !getActiveConnection()->m_webDavConnection) {
        if (m_websocketMessengers.contains(getActiveConnection()->m_name)) {
            m_websocketMessengers[getActiveConnection()->m_name]->disconnectFromServer();
        }
    } else if (getActiveConnection() != nullptr) {
        if (m_webDavMessengers.contains(getActiveConnection()->m_name)) {
            m_webDavMessengers[getActiveConnection()->m_name]->disconnectFromServer();
        }
    } else {
        qCWarning(client) << "No active connection";
    }
}

void Client::getNetworkDirectory(const QString &dirPath)
{
    qCDebug(client) << "Directory request";

    bool error{false};
    if (getActiveConnection() != nullptr && !getActiveConnection()->m_webDavConnection) {
        if (m_websocketMessengers.contains(getActiveConnection()->m_name)) {
            m_websocketMessengers[getActiveConnection()->m_name]->getNetworkDirectory(dirPath);
        } else {
            error = true;
        }
    } else if (getActiveConnection() != nullptr) {
        if (m_webDavMessengers.contains(getActiveConnection()->m_name)) {
            m_webDavMessengers[getActiveConnection()->m_name]->getNetworkDirectory(dirPath);
        } else {
            error = true;
        }
    }
    if (error) {
        qCWarning(client) << "Directory create request error: Connection doesn't exist";
    }
}

void Client::getSystemDirectory(const QString &dirPath)
{
    auto newActiveDirectory = fromJsonToFileInfo(createJsonFromDirectory(dirPath));
    m_systemDirectoryManager.updateDirectory(std::move(newActiveDirectory), dirPath);

    emit currentSystemDirectoryChanged();
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
                    << m_networkDirectoryManager.getActiveDirectory()[fileIndex]->m_path;

    if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
        return;
    }

    const QString downloadID = generateDownloadID(fileIndex);

    auto activeConnection = getActiveConnection();
    auto fileInfo = m_networkDirectoryManager.getActiveDirectory()[fileIndex];
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

    if (getActiveConnection()->m_webDavConnection) {
        m_webDavTransports[downloadID] = new ClientWebDAV(getActiveConnection(),
                                                          m_networkDirectoryManager);
        m_webDavTransports[downloadID]->setDownloadInfo(
            m_downloadManager.getDownloadInfoDict()[downloadID]);

        connect(m_webDavTransports[downloadID],
                &ClientWebDAV::fileReceived,
                this,
                &Client::fileReceived);

        m_webDavTransports[downloadID]->getFile(m_downloadManager.getDownloadInfoDict()[downloadID]);
    } else {
        m_tcpTlsTransports[downloadID] = new ClientTcpTlsTransport(savePath, this);

        connect(m_tcpTlsTransports[downloadID],
                &ClientTcpTlsTransport::fileReceived,
                this,
                &Client::fileReceived);

        m_tcpTlsTransports[downloadID]->getFile(getActiveConnection()->m_address,
                                                getActiveConnection()->m_transportPort,
                                                m_downloadManager.getDownloadInfoDict()[downloadID]);
    }
}

// void Client::startDownload(int /*downloadIndex*/)
// {
//     if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
//         return;
//     }

//     // m_tcpTlsTransports[downloadID].start(/**/);
// }

// void Client::stopDownload(int /*downloadIndex*/)
// {
//     // m_tcpTlsTransports[downloadID].stop(/**/);
// }

// void Client::connectToRelayServer()
// {
//     qCDebug(client) << "Connection check realy server request";

//     // m_websocketMessengers[getActiveConnection()->m_name].connectToRelayServer(getActiveConnection());
// }

// void Client::getFileFromRelayServer(int fileIndex,
//                                     const QString &userName,
//                                     const QString &saveName,
//                                     const QString &savePath)
// {
//     qCDebug(client) << "Getting file:"
//                     << m_networkDirectoryManager.getActiveDirectory()[fileIndex]->m_path;

//     if (getActiveConnection()->m_connectionState != ConnectionInfo::ConnectionState::Connected) {
//         return;
//     }

//     const QString downloadID = generateDownloadID(fileIndex);

//     auto activeConnection = getActiveConnection();
//     auto fileInfo = m_networkDirectoryManager.getActiveDirectory()[fileIndex];
//     m_downloadManager.addDownload(downloadID,
//                                   activeConnection->m_address,
//                                   activeConnection->m_hostKey,
//                                   fileInfo->m_name,
//                                   fileInfo->m_path,
//                                   saveName,
//                                   savePath,
//                                   fileInfo->m_size,
//                                   0,
//                                   fileInfo->m_created,
//                                   fileInfo->m_modified,
//                                   fileInfo->m_accessed,
//                                   DownloadInfo::DownloadState::Wait);

//     connect(m_tcpTlsTransports[downloadID],
//             &ClientTcpTlsTransport::fileReceived,
//             this,
//             &Client::fileReceived);

//     m_tcpTlsTransports[downloadID]
//         ->getFileFromRelay(getActiveConnection()->m_address,
//                            getActiveConnection()->m_transportPort,
//                            userName,
//                            m_downloadManager.getDownloadInfoDict()[downloadID]);
// }

void Client::updateConnection(ConnectionInfo *connectionInfo)
{
    m_connectionManager.updateConnection(connectionInfo);

    if (!m_isActiveConnectionConnected) {
        m_isActiveConnectionConnected = true;
        qCritical(client) << "UpdateConnection";
        emit connected();
    } else {
        emit disconnected();
    }
}

ConnectionInfo *Client::getActiveConnection()
{
    qCDebug(client) << "Getting an active connection";

    return m_connectionManager.getActiveConnection();
}

void Client::setConnectionPreferences()
{
    qCDebug(client) << "Changing connection preferences";

    if (!getActiveConnection()->m_webDavConnection) {
        connectSignals();
    } else {
        connectWebDavSignals();
    }

    qCInfo(client) << "New connection settings have been applied.";
}

QString Client::generateDownloadID(int fileIndex)
{
    QString downloadID
        = QCryptographicHash::hash((m_networkDirectoryManager.getActiveDirectory()[fileIndex]->m_path
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

void Client::connectWebDavSignals()
{
    connect(m_webDavMessengers[getActiveConnection()->m_name],
            &ClientWebDAV::statusCodeChanged,
            this,
            &Client::connectionStatusCodeChanged);

    connect(m_webDavMessengers[getActiveConnection()->m_name],
            &ClientWebDAV::currentDirectoryChanged,
            this,
            &Client::onCurrentDirectoryChanged);

    connect(m_webDavMessengers[getActiveConnection()->m_name],
            &ClientWebDAV::connected,
            this,
            &Client::updateConnection);
    connect(m_webDavMessengers[getActiveConnection()->m_name],
            &ClientWebDAV::disconnected,
            this,
            &Client::updateConnection);
}

void Client::onCurrentDirectoryChanged()
{
    emit currentDirectoryChanged(m_networkDirectoryManager.getActiveDirectory());
}

void Client::onActiveConnectionChanged()
{
    m_networkDirectoryManager.clearActiveDirectory();
}
