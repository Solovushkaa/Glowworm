#ifndef CLIENTHTTPMESSENGER_HPP
#define CLIENTHTTPMESSENGER_HPP

#include <QNetworkAccessManager>
#include "client_messenger.hpp"

class ClientHttpMessenger : public ClientMessenger
{
    Q_OBJECT

public:
    ClientHttpMessenger();

    void checkConnectionToServer(ConnectionInfo *connectionInfo) override;
    // void getDirectoryList(ConnectionInfo *connectionInfo) override;

private slots:
    void onConnectionStatusReceived() override;
    // void onDirectoryReceived() override;

    // signals:
    //     void statusCodeChanged(int statusCode);

    /* Members */
private:
    QNetworkAccessManager *m_networkManager;
    QTcpSocket tcpSocket;
};

#endif // CLIENTHTTPMESSENGER_HPP
