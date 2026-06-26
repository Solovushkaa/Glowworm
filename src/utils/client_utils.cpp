#include "client_utils.hpp"
#include <QDataStream>
#include <QIODevice>

QuickConnectionInfo parseConnectionKey(const QString &connectionKey)
{
    QByteArray data = QByteArray::fromBase64(connectionKey.toUtf8());
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_6_0);

    QByteArray hostIp;
    QByteArray serverCert;
    quint16 messengerPort, transportPort;
    quint8 relayServerID;
    stream >> hostIp;
    stream >> messengerPort;
    stream >> transportPort;
    stream >> relayServerID;
    stream >> serverCert;

    QuickConnectionInfo info;
    info.ip = QString::fromUtf8(hostIp);
    info.messengerPort = messengerPort;
    info.transportPort = transportPort;
    info.relayServerID = relayServerID;
    info.serverCertFingerprint = serverCert;

    return info;
}
