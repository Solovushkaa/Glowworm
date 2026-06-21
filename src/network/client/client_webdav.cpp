#include "client_webdav.hpp"

Q_LOGGING_CATEGORY(client_webdav, "client.webdav")

ClientWebDAV::ClientWebDAV(ConnectionInfo *connectionInfo,
                           DirectoryManager &directoryManager,
                           ClientWebDAVProtocol::WebdavConnectionType webDavProtocol,
                           QObject *parent)
    : QObject(parent)
    , r_directoryManager(directoryManager)
{
    r_connectionInfo = connectionInfo;

    m_webDavProtocol.setConnectionSettings(webDavProtocol,
                                           r_connectionInfo->m_address,
                                           "/",
                                           r_connectionInfo->m_webDavUsername,
                                           r_connectionInfo->m_webDavPassword,
                                           r_connectionInfo->m_webDavPort);

    connect(&m_webDavParser, &WebdavDirParser::errorChanged, this, &ClientWebDAV::printError);
    connect(&m_webDavProtocol, &ClientWebDAVProtocol::errorChanged, this, &ClientWebDAV::printError);

    m_pingTimer.setInterval(5000); // 5 sec
    connect(&m_pingTimer, &QTimer::timeout, this, &ClientWebDAV::sendPing);

    connectToServer();

    qCDebug(client_webdav) << "ClientWebDAV - created";
}

ClientWebDAV::~ClientWebDAV()
{
    qCDebug(client_webdav) << "ClientWebDAV - destroyed";
}

void ClientWebDAV::connectToServer()
{
    qCritical() << "ClientWebDAV::connectToServer()";

    auto reply = m_webDavProtocol.get("/");
    connect(reply, &QNetworkReply::finished, this, &ClientWebDAV::onConnected);
}

void ClientWebDAV::disconnectFromServer()
{
    if (!(r_connectionInfo->m_connectionState == ConnectionInfo::ConnectionState::Connected)) {
        return;
    }

    m_missedPongs = 3;

    emit disconnected(r_connectionInfo);
}

void ClientWebDAV::sendPing()
{
    auto reply = m_webDavProtocol.get("/");
    connect(reply, &QNetworkReply::finished, this, &ClientWebDAV::onPong);

    QTimer::singleShot(m_pongTimeout, this, [this]() {
        ++m_missedPongs;
        if (m_missedPongs >= 2) {
            qWarning() << "Connection dead, closing...";
        }
    });
}

void ClientWebDAV::getDirectory(const QString &dirPath)
{
    m_path = dirPath;
    qDebug() << "Get directory";
    bool active = m_webDavParser.listDirectory(&m_webDavProtocol, dirPath);

    if (active) {
        connect(&m_webDavParser,
                &WebdavDirParser::directoryReceived,
                this,
                &ClientWebDAV::onDirectoryReceived);
    }
}

void ClientWebDAV::getFile(DownloadInfo *downloadInfo)
{
    r_downloadInfo = downloadInfo;
    // if (!(r_connectionInfo->m_connectionState == ConnectionInfo::ConnectionState::Connected)) {
    // return;
    // } else {
    m_saveFile = new QFile(downloadInfo->m_savePath);
    if (!m_saveFile->open(QIODevice::WriteOnly)) {
        qCWarning(client_webdav) << "Error:" << m_saveFile->errorString();
    } else {
        qCCritical(client_webdav) << "Open:" << m_saveFile->fileName();
    }

    auto reply = m_webDavProtocol.get(downloadInfo->m_path,
                                      m_saveFile,
                                      downloadInfo->m_lastReceivedByte);

    connect(reply, &QNetworkReply::downloadProgress, this, &ClientWebDAV::onDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &ClientWebDAV::onFileReceived);
    // }
}

void ClientWebDAV::setDownloadInfo(DownloadInfo *downloadInfo)
{
    r_downloadInfo = downloadInfo;
}

void ClientWebDAV::onConnected()
{
    qInfo() << "Connected to server";

    r_connectionInfo->setConnectionState(ConnectionInfo::ConnectionState::Connected);
    emit connected(r_connectionInfo);

    sendPing();
    m_pingTimer.start();
    qDebug() << "Start ping";
}

void ClientWebDAV::onDisconnected()
{
    qInfo() << "Disconnected from server";

    r_connectionInfo->setConnectionState(ConnectionInfo::ConnectionState::Disconnected);
    emit disconnected(r_connectionInfo);

    m_pingTimer.stop();
}

void ClientWebDAV::onPong()
{
    auto reply = qobject_cast<QNetworkReply *>(sender());

    m_missedPongs = 0;
    qDebug().nospace() << "RTT WebDAV Disk: - ms"; // TODO: Add custom ping checker
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        qWarning() << "Packet received but corrupted";
    } else {
        emit statusCodeChanged(200); // TODO:
    }
}

void ClientWebDAV::onDirectoryReceived()
{
    QList<WebdavItem> directoryList = m_webDavParser.getList();

    std::vector<std::unique_ptr<FileInfo>> filesInfo;

    for (auto &fileItem : directoryList) {
        filesInfo.push_back(std::make_unique<FileInfo>(fileItem.name(),
                                                       fileItem.path(),
                                                       fileItem.createdAt().toString(),
                                                       fileItem.createdAt().toString(),
                                                       fileItem.createdAt().toString(),
                                                       fileItem.size(),
                                                       fileItem.isDir(),
                                                       true));
    }

    r_directoryManager.updateDirectory(std::move(filesInfo), m_path);

    emit currentDirectoryChanged();
}

void ClientWebDAV::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug().nospace() << "Download progress: " << bytesReceived << "/" << bytesTotal;
    r_downloadInfo->setLastReceivedByte(bytesReceived);
}

void ClientWebDAV::onFileReceived()
{
    r_downloadInfo->setDownloadState(DownloadInfo::DownloadState::Finish);
    r_downloadInfo->m_forDelete = true;

    emit fileReceived(r_downloadInfo->m_savePath);
}

void ClientWebDAV::printError(QString error)
{
    qWarning() << "Error:" << error;
}
