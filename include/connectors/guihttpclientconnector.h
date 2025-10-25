#ifndef GUIHTTPCLIENTCONNECTOR_H
#define GUIHTTPCLIENTCONNECTOR_H

#include <QQuickItem>
#include "httpclient.h"

class GUIHttpClientConnector : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    GUIHttpClientConnector();

private:
    HttpClient m_httpClient;
};

#endif // GUIHTTPCLIENTCONNECTOR_H
