#ifndef CLIENTHTTPMESSENGER_HPP
#define CLIENTHTTPMESSENGER_HPP

#include <QNetworkAccessManager>
#include "client_messenger.hpp"
#include "directory_manager.hpp"

class ClientHttpMessenger : public ClientMessenger
{
    Q_OBJECT

public:
    explicit ClientHttpMessenger(DirectoryManager &directoryManager);

    void checkConnectionToServer(ConnectionInfo *connectionInfo) override;
    void getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath) override;

private slots:
    void onConnectionStatusCodeReceived() override;
    void onDirectoryReceived() override;

    // signals:
    //     void statusCodeChanged(int statusCode);

    /* Members */
private:
    QNetworkAccessManager m_networkManager;
    DirectoryManager &r_directoryManager;
};

#endif // CLIENTHTTPMESSENGER_HPP
