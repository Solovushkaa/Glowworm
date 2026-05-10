#ifndef CLIENTMESSENGER_HPP
#define CLIENTMESSENGER_HPP

#include "connection_info.hpp"

class ClientMessenger : public QObject
{
    Q_OBJECT

public:
    // ClientMessenger() = default;
    virtual ~ClientMessenger() = default;

    virtual void checkConnectionToServer(ConnectionInfo *) = 0;
    // virtual void getDirectoryList(ConnectionInfo *) = 0;

signals:
    void requestFinished();
    void requestError(const QString &error);

    void statusCodeChanged(int statusCode);
    // void currentHostChanged(QString &&hostKey);
    // void currentDirectoryChanged(QList<QVariantHash> &&currentDirectory);

protected slots:
    virtual void onConnectionStatusReceived() = 0;
    // virtual void onDirectoryReceived() = 0;
};

#endif // CLIENTMESSENGER_HPP
