#ifndef CLIENTWEBDAV_HPP
#define CLIENTWEBDAV_HPP

#include <QFile>
#include "directory_manager.hpp"
#include "webdav_dir_parser.hpp"

class ClientWebDAV : public QObject
{
    Q_OBJECT

public:
    ClientWebDAV(const QString &address,
                 quint16 port,
                 const QString &username,
                 const QString &password,
                 DirectoryManager &directoryManager,
                 ClientWebDAVProtocol::WebdavConnectionType webDavProtocol
                 = ClientWebDAVProtocol::WebdavConnectionType::HTTPS,
                 QObject *parent = nullptr);
    // ~ClientWebDAV();

    void sendPing();
    void getDirectory(const QString &dirPath);

    void getFile();

public slots:
    // void onPong();

    void directoryReceived();
    void printError(QString error);
    // void replySkipRead();

signals:
    void currentDirectoryChanged();

private:
    ClientWebDAVProtocol m_webDavProtocol;
    WebdavDirParser m_webDavParser;
    QList<QNetworkReply *> m_replyList;
    QString m_path;
    QFile m_saveFile;
    DirectoryManager &r_directoryManager;
};

#endif // CLIENTWEBDAV_HPP
