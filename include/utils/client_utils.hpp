#ifndef CLIENTUTILS_HPP
#define CLIENTUTILS_HPP

#include <QString>

struct QuickConnectionInfo
{
    QString ip;
    quint16 messengerPort;
    quint16 transportPort;
    quint8 relayServerID;
    QByteArray serverCertFingerprint;
};

QuickConnectionInfo parseConnectionKey(const QString &connectionKey);

#endif // CLIENTUTILS_HPP
