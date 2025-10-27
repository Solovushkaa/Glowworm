#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
    , m_connManager()
    , m_downloadManager()
    , m_httpClient()
{
    m_currentProtocol = &m_httpClient;

    // connect(&m_timer, &QTimer::timeout, this, &HttpClient::startConnectionVerification);
}

void Client::checkConnectionToServer()
{
    
}
