#ifndef APP_H
#define APP_H

#include <QObject>
#include "guihttpclientconnector.h"
#include "guihttpserverconnector.h"
#include "initialize.h"

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

private:
    Initialize m_initializer;
    GUIHttpClientConnector m_guiHttpClientConnector;
    GUIHttpServerConnector m_guiHttpServerConnector;
};

#endif // APP_H
