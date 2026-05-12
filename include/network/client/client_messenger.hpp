#ifndef CLIENTMESSENGER_HPP
#define CLIENTMESSENGER_HPP

#include "connection_info.hpp"

class ClientMessenger : public QObject
{
    Q_OBJECT

public:
    virtual void checkConnectionToServer(ConnectionInfo *connectionInfo) = 0;
    virtual void getDirectory(ConnectionInfo *connectionInfo, const QString &dirPath) = 0;

signals:
    void requestFinished();
    void requestError(const QString &error);

    void statusCodeChanged(int statusCode);
    // void currentHostChanged(QString &&hostKey);
    void currentDirectoryChanged();

protected slots:
    virtual void onConnectionStatusCodeReceived() = 0;
    virtual void onDirectoryReceived() = 0;
};

#endif // CLIENTMESSENGER_HPP
