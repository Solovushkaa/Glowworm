#include "server_utils.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QSslCertificate>
#include <QStandardPaths>
#include <QUuid>

QString createHostKey()
{
    qDebug() << "Generating host key";

    QString hostKey;
    hostKey.reserve(256);

    hostKey.append(QSysInfo::prettyProductName());
    hostKey.append(QSysInfo::currentCpuArchitecture());
    hostKey.append(QSysInfo::kernelType());
    hostKey.append(QSysInfo::kernelVersion());
    hostKey.append(QSysInfo::machineHostName());
    hostKey.append(QSysInfo::machineUniqueId().toHex());

    hostKey = QCryptographicHash::hash(hostKey.toUtf8(), QCryptographicHash::Sha256).toHex();

    qInfo().nospace().noquote() << R"(HostKey: ")" << hostKey.slice(14) << R"(..." was generated)";

    return hostKey;
}

std::pair<qint64, qint64> parseRange(const QString &rangeRequestValue)
{
    std::pair<qint64, qint64> range(-1, -1);

    static const QRegularExpression rx("^bytes=(\\d*)-(\\d*)$");
    QRegularExpressionMatch match = rx.match(rangeRequestValue);

    if (match.hasMatch()) {
        qint64 first = match.captured(1).toLongLong();
        qint64 second = match.captured(2).toLongLong();
        if (second > first) {
            range.first = first;
            range.second = second;
        } else {
            return {-1, -1};
        }
    }

    return range;
}

QString getActiveLocalIPv4()
{
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if ((iface.flags() & QNetworkInterface::IsLoopBack)
            || !(iface.flags() & QNetworkInterface::IsUp)
            || !(iface.flags() & QNetworkInterface::IsRunning))
            continue;

        const QString name = iface.humanReadableName().toLower();
        if (name.contains("vmnet") || name.contains("vbox") || name.contains("docker")
            || name.contains("veth"))
            continue;

        for (auto &entry : iface.addressEntries()) {
            QHostAddress addr = entry.ip();
            if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()
                && !addr.isLinkLocal()) {
                return addr.toString();
            }
        }
    }
    return QString();
}

QString createConnectionKey(quint16 messengerPort, quint16 transportPort, quint8 relayServerID)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_0);

    stream << getActiveLocalIPv4().toUtf8();
    stream << messengerPort;
    stream << transportPort;
    stream << relayServerID;

    const QString serverCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    QFile serverCertFile(serverCertPath);
    if (!serverCertFile.open(QIODevice::ReadOnly)) {
        qFatal() << serverCertFile.errorString();
    }

    QSslCertificate sslCertificate(serverCertFile.readAll(), QSsl::Pem);

    qCritical() << sslCertificate.digest(QCryptographicHash::Sha256);

    stream << sslCertificate.digest(QCryptographicHash::Sha256);

    return data.toBase64();
}
