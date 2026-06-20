#include "client_webdav.hpp"

ClientWebDAV::ClientWebDAV(const QString &address,
                           quint16 port,
                           const QString &username,
                           const QString &password,
                           DirectoryManager &directoryManager,
                           ClientWebDAVProtocol::WebdavConnectionType webDavProtocol,
                           QObject *parent)
    : QObject(parent)
    , r_directoryManager(directoryManager)
{
    m_webDavProtocol.setConnectionSettings(webDavProtocol, address, "/", username, password, port);

    connect(&m_webDavParser, &WebdavDirParser::finished, this, &ClientWebDAV::directoryReceived);
    connect(&m_webDavParser, &WebdavDirParser::errorChanged, this, &ClientWebDAV::printError);
    connect(&m_webDavProtocol, &ClientWebDAVProtocol::errorChanged, this, &ClientWebDAV::printError);
}

void ClientWebDAV::sendPing()
{
    // m_webDavParser.listDirectory(&m_webDavProtocol, "/");
}

void ClientWebDAV::getDirectory(const QString &dirPath)
{
    m_path = dirPath;
    qDebug() << "Get directory";
    m_webDavParser.listDirectory(&m_webDavProtocol, dirPath);
}

void ClientWebDAV::directoryReceived()
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

void ClientWebDAV::printError(QString error)
{
    qDebug() << "Error:" << error;
}
