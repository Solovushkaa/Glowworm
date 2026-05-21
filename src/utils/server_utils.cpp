#include "server_utils.hpp"
#include <QCryptographicHash>
#include <QDebug>
#include <QRegularExpression>

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
