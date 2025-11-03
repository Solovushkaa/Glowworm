#ifndef JSONHELPERFUNCTIONS_H
#define JSONHELPERFUNCTIONS_H

#include <QObject>

QByteArray createJsonFromDirectory(const QString &path);
QList<QVariantHash> fromJsonToHash(QByteArray &data);

#endif // JSONHELPERFUNCTIONS_H
