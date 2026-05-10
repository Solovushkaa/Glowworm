#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <QString>

QString createHostKey();

std::pair<qint64, qint64> parseRange(const QString &rangeRequestValue);

#endif // SERVERUTILS_HPP
