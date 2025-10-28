#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "clientsavedconnectionmanager.h"
#include "connectioninfo.h"
#include "httpclient.h"
#include "unfinisheddownloadmanager.h"

// A class for combining the entire client part of the application.
// The class sends requests to the server and receives responses,
// as well as interacts with the GUI
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

    ConnectionInfo m_connInfo;
    BaseClient *m_currentProtocol;

    QList<QVariantHash> m_currentDirectory;
    // QString m_currentHostKey;

    /* Verifications */
private:
    void startConnectionVerification()
    {
        checkConnectionToServer();
        m_timer.start(m_pingTime);
    }
    void stopConnectionVerification() { m_timer.stop(); }

    QTimer m_timer;
    int m_pingTime = 5'000;
};

#endif // CLIENT_H
