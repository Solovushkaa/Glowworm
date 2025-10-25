#ifndef APP_H
#define APP_H

#include <QObject>
#include "httpclient.h"
#include "httpserver.h"
#include "initialize.h"

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

private:
    Initialize m_initializer;
    HttpClient m_httpClient;
    HttpServer m_httpServer;
};

#endif // APP_H
