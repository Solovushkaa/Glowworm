#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QFile>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTimer>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>
#include "clientsavedconnectionmanager.h"
#include "unfinisheddownloadmanager.h"

class HttpClient : public QObject
{
    Q_OBJECT
    QML_SINGLETON

public:
    explicit HttpClient(QObject *parent = nullptr);

    Q_INVOKABLE void checkConnectionToServer();

    Q_INVOKABLE QList<QVariantHash> getCurrentDirectory() { return m_currentDirectory; }

    // Будет изменён. Хранение будет в ConnectionInfo
    Q_INVOKABLE void setUrl(const QUrl &newUrl) { m_url = newUrl; }

    Q_INVOKABLE void getDirectoryList(const QString &path);
    Q_INVOKABLE void getFile(const QString &path, const QString &savePath, const QString &saveName);

    Q_INVOKABLE void startDownload(const QString &downloadID);
    Q_INVOKABLE void stopDownload(const QString &downloadID);

    Q_INVOKABLE QList<QVariantHash> getUnfinishedDownloads()
    {
        return m_downloadManager.getVariantListUnfinishedDownloads(
            m_downloadManager.get_dowloadInfoList());
    }

private:
    void requestAnswersHandler();

signals:
    void requestSuccessfulFinished();
    void rangeRequestSuccessfulFinished(const QString &path);
    void requestError(const QString &error);

    void newDownload(const QString &name, const QString &path, qint64 size);
    void changeProgress(const QString &downloadID, qint64 bytesReceived);
    void stopProgress(const QString &downloadID, qint64 bytesReceived);
    void changeStatusCode(int currentStatusCode);

private slots:
    void onCheckConnectionReceived();
    void onDirectoryReceived();
    void onRangeReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /* Members */
private:
    QNetworkAccessManager *m_networkManager;
    QUrl m_url;

    QList<QVariantHash> m_currentDirectory;
    QString m_currentHostKey;

    // Managers
    UnfinishedDownloadManager m_downloadManager;
    ClientSavedConnectionManager m_connManager;

public:
    QList<QString> m_hostKeys;

    /* Verifications */
public:
    Q_INVOKABLE void startConnectionVerification()
    {
        checkConnectionToServer();
        m_timer.start(m_pingTime);
    }
    Q_INVOKABLE void stopConnectionVerification() { m_timer.stop(); }

private:
    QTimer m_timer;
    int m_pingTime = 5'000;
};

#endif // HTTPCLIENT_H
