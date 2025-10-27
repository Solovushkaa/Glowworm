#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QObject>

class ConnectionInfo : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionInfo(QObject *parent = nullptr);

signals:
};

#endif // CONNECTIONINFO_H
