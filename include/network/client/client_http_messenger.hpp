#ifndef CLIENTHTTPMESSENGER_HPP
#define CLIENTHTTPMESSENGER_HPP

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "client_connection_manager.hpp"
#include "directory_manager.hpp"

Q_DECLARE_LOGGING_CATEGORY(client_http_messenger)

class ClientHttpMessenger : public QObject
{
    Q_OBJECT

public:
    explicit ClientHttpMessenger(ClientConnectionManager &clientConnectionManager,
                                 DirectoryManager &directoryManager);
    ~ClientHttpMessenger();

    void checkConnectionToServer(ConnectionInfo *connectionInfo);
    void getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath);

private:
    void networkErrorHandler(const QNetworkReply::NetworkError networkError);

private slots:
    void onConnectionStatusCodeReceived();
    void onDirectoryReceived();

signals:
    void requestConnectionStatusReceivedError();
    void requestDirectoryReceivedError();

    void statusCodeChanged(int statusCode);
    void currentDirectoryChanged();

    // signals:
    //     void statusCodeChanged(int statusCode);

    /* Members */
private:
    QNetworkAccessManager m_networkManager;
    ClientConnectionManager &r_clientConnectionManager;
    DirectoryManager &r_directoryManager;
};

#endif // CLIENTHTTPMESSENGER_HPP
