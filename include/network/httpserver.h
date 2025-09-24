#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QtHttpServer/QHttpServer>
#include <QStringLiteral>
#include <memory>
#include <QTcpServer>
#include <QCoreApplication>

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);

    void startHttpServer();

private:
    void routeConnection();
    void routeFileSystem();

    std::pair<qint64, qint64> parseRange(const QString &rangeRequestValue);
    QString getHostKey();
    bool createHostKey();

signals:

private:
    QHttpServer m_httpServer;
    std::unique_ptr<QTcpServer> m_tcpServer;
    QString m_hostKey;
};

#endif // HTTPSERVER_H
