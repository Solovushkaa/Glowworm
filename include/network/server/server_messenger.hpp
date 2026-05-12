#ifndef SERVERMESSENGER_HPP
#define SERVERMESSENGER_HPP

#include <QObject>

class ServerMessenger : public QObject
{
    Q_OBJECT

public:
    virtual bool start(bool useDefaultConfig, bool useSecureConfig) = 0;
    virtual void stop(bool stopDefaultConfig, bool stopSecureConfig) = 0;

signals:
    void serverMessengerStarted(QString serverMessengerName);
    void serverMessengerStoped(QString serverMessengerName);
};

#endif // SERVER_MESSENGER_HPP
