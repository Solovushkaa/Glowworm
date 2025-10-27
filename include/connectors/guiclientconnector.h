#ifndef GUICLIENTCONNECTOR_H
#define GUICLIENTCONNECTOR_H

#include <QObject>
// #include "client.h"

class GUIClientConnector : public QObject
{
    Q_OBJECT
public:
    explicit GUIClientConnector(QObject *parent = nullptr);

signals:
};

#endif // GUICLIENTCONNECTOR_H
