#include "httpserver.h"
#include <QCryptographicHash>
#include <QFile>
#include <QStandardPaths>
#include <QSysInfo>
#include "jsonhelperfunctions.h"

struct RestPath {
    QString s;
    RestPath() = default;
    RestPath(const QString &x) : s(x) {}
};

HttpServer::HttpServer(QObject *parent)
        : QObject{parent}
{
}

void HttpServer::startHttpServer()
{
    m_httpServer.router()->addConverter<RestPath>(u".+");

    routeConnection();
    routeFileSystem();

    m_tcpServer = std::make_unique<QTcpServer>();
    if (!m_tcpServer->listen(QHostAddress::LocalHost, 1234)
        || !m_httpServer.bind(m_tcpServer.get())) {
        qWarning() << "QHttpServer. Server failed to listen on a port.";
        return;
    }
    quint16 port = m_tcpServer->serverPort();
    m_tcpServer.release();

    qDebug() << "Listen" << " on port: " << port;
}

void HttpServer::routeConnection()
{
    m_httpServer.route("/",
                       QHttpServerRequest::Method::Head,
                       [this](const QHttpServerRequest &request) {
                           qDebug() << request.query().toString();
                           qDebug() << request.localAddress();
                           qDebug() << request.url().path();

                           QHttpServerResponse response("");

                           QHttpHeaders responseHeader;
                           responseHeader.append(QHttpHeaders::WellKnownHeader::Server,
                                                 getHostKey());

                           response.setHeaders(responseHeader);

                           return response;
                       });
}

void HttpServer::routeFileSystem()
{
    m_httpServer
        .route("/?<arg>",
               QHttpServerRequest::Method::Get,
               [this](const RestPath &path, const QHttpServerRequest &request) {
                   qDebug() << "GET handler debug info: ";
                   qDebug() << "-----------------------------------";
                   qDebug() << (path.s != '/' ? "/" + path.s : path.s);
                   qDebug() << request.url();

                   auto headers = request.headers();
                   if (headers.contains("Accept")) {
                       qDebug() << "Accept:" << headers.value("Accept");

                       if (headers.value("Accept") == "application/json") {
                           qDebug() << "-----------------------------------";
                           QHttpServerResponse response(createJsonFromDirectory("/" + path.s));

                           return response;

                       } else if (headers.value("Accept") == "application/octet-stream") {
                           qDebug() << "Ответ на range запрос";
                           if (headers.contains("Range")) {
                               qDebug() << "Range: " << headers.value("Range");

                               auto [start, end] = parseRange(
                                   QString(headers.value("Range").toByteArray()));

                               if (start == -1 || end == -1) { // -1 == max(qint64)
                                   qDebug() << "-----------------------------------";
                                   return QHttpServerResponse(
                                       QHttpServerResponse::StatusCode::BadRequest);
                               }

                               qDebug() << "-----------------------------------";

                               QFile file("/" + path.s);
                               if (!file.open(QIODevice::ReadOnly)) {
                                   qWarning() << "Ошибка доступа к файлу: " << file.fileName();
                               }

                               if (start > end) {
                                   qWarning()
                                       << "Некорректные границы файла: start(" << start << "), end("
                                       << end << "), file size(" << file.size() << ")";
                               }

                               if (!file.seek(start)) {
                                   qWarning() << "Ошибка чтения с позиции: " << start;
                               }

                               end = std::min(end, file.size() - 1);

                               QByteArray data = file.read(end - start + 1);

                               QHttpServerResponse response(data);
                               QHttpHeaders responseHeader;

                               responseHeader.append(QHttpHeaders::WellKnownHeader::ContentRange,
                                                     "bytes " + QString::number(start) + "-"
                                                         + QString::number(end) + "/"
                                                         + QString::number(file.size()));

                               responseHeader.append(QHttpHeaders::WellKnownHeader::Server,
                                                     getHostKey());

                               response.setHeaders(responseHeader);

                               return response;
                           }
                       }
                   }
                   qDebug() << "-----------------------------------";
                   return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
               });
}

std::pair<qint64, qint64> HttpServer::parseRange(const QString &rangeRequestValue)
{
    std::pair<qint64, qint64> range(-1,-1);

    QRegularExpression rx("^bytes=(\\d*)-(\\d*)$");
    QRegularExpressionMatch match = rx.match(rangeRequestValue);

    if(match.hasMatch()){
        range.first = match.captured(1).toLongLong();
        range.second = match.captured(2).toLongLong();
        if(range.second > range.first){
            return range;
        } else {
            return {-1,-1};
        }
    }

    return range;
}

QString HttpServer::getHostKey()
{
    if (m_hostKey.size() == 0) {
        QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                   + "/AppData/Server/hostKey.txt");

        if (!file.open(QIODevice::ReadOnly)) {
            if (!createHostKey()) {
                return QString();
            }
        } else {
            m_hostKey = file.readAll();
            if (m_hostKey.size() == 0) {
                createHostKey();
            }
            if (m_hostKey[m_hostKey.size() - 1] == '\n') {
                m_hostKey = m_hostKey.chopped(2);
            }
        }
    }

    return m_hostKey;
}

bool HttpServer::createHostKey()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
               + "/AppData/Server/hostKey.txt");

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Ошибка открытия для записи файла hostKey.txt";
        return false;
    }

    QStringList components;

    components.append(QSysInfo::productType());
    components.append(QSysInfo::currentCpuArchitecture());
    components.append(QSysInfo::kernelType());
    components.append(QSysInfo::prettyProductName());

    QString combined = components.join("|");
    QByteArray hostKeyHash
        = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256).toHex().left(10);

    file.write(hostKeyHash);

    m_hostKey = hostKeyHash;

    return true;
}
