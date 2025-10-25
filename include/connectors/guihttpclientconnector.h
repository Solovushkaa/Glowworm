#ifndef GUIHTTPCLIENTCONNECTOR_H
#define GUIHTTPCLIENTCONNECTOR_H

#include <QQuickItem>
#include "httpclient.h"

class GUIHttpClientConnector : public QQuickItem
{
    Q_OBJECT
    QML_SINGLETON

public:
    GUIHttpClientConnector();

    Q_INVOKABLE void checkConnectionToServer();

    Q_INVOKABLE QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }
    Q_INVOKABLE void setUrl(const QUrl &newUrl) { m_url = newUrl; }

    Q_INVOKABLE void getDirectoryList(const QString &path);
    Q_INVOKABLE void getFile(const QString &path, const QString &savePath, const QString &saveName);

    Q_INVOKABLE void startDownload(const QString &downloadID);
    Q_INVOKABLE void stopDownload(const QString &downloadID)
    {
        return m_httpClient.stopDownload(downloadID);
    }

    Q_INVOKABLE QList<QVariantHash> getUnfinishedDownloads()
    {
        return m_httpClient.getUnfinishedDownloads();
    }

private:
    HttpClient m_httpClient;
};

#endif // GUIHTTPCLIENTCONNECTOR_H
