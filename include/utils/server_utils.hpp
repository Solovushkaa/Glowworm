#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <QString>

QString createHostKey();

std::pair<qint64, qint64> parseRange(const QString &rangeRequestValue);

QString getActiveLocalIPv4();

QString createConnectionKey(quint16 messengerPort, quint16 transportPort, quint8 relayServerID);

#endif // SERVERUTILS_HPP
