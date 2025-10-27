#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QNetworkAccessManager>
#include <QObject>
#include "baseclient.h"

class HttpClient : public BaseClient
{
    Q_OBJECT

public:
    HttpClient(QObject *parent = nullptr);

    void checkConnectionToServer();

    void getDirectoryList(const QString &path) override;
    void getFile(const QString &path, const QString &savePath, const QString &saveName) override;

    void startDownload(const QString &downloadID) override;
    void stopDownload(const QString &downloadID) override;

private slots:
    void onConnectionStatusReceived() override;
    void onDirectoryReceived() override;
    void onFileReceived() override;
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) override;

    /* Members */
private:
    QNetworkAccessManager *m_networkManager;
};

#endif // HTTPCLIENT_H
