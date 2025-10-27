#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "clientsavedconnectionmanager.h"
#include "connectioninfo.h"
#include "httpclient.h"
#include "unfinisheddownloadmanager.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    void checkConnectionToServer();

    QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }

signals:

private:
    ClientSavedConnectionManager m_connManager;
    UnfinishedDownloadManager m_downloadManager;
    HttpClient m_httpClient;

private:
    ConnectionInfo m_connInfo;
    BaseClient *m_currentProtocol;

    QList<QVariantHash> m_currentDirectory;
    // QString m_currentHostKey;

    /* Verifications */
public:
    void startConnectionVerification()
    {
        checkConnectionToServer();
        m_timer.start(m_pingTime);
    }
    void stopConnectionVerification() { m_timer.stop(); }

private:
    QTimer m_timer;
    int m_pingTime = 5'000;
};

#endif // CLIENT_H
