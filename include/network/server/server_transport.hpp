#ifndef SERVERTRANSPORT_HPP
#define SERVERTRANSPORT_HPP

#include <QObject>

class ServerTransport : public QObject
{
    Q_OBJECT

public:
    // explicit ServerTransport(QObject *parent = nullptr)
    //     : QObject(parent)
    // {}

    virtual bool startdownload();
    virtual bool stopDownload();

signals:
    void downloadStarted();
    void downloadEnded();
};

#endif // SERVERTRANSPORT_HPP
