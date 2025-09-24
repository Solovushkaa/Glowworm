#ifndef JSONHELPERFUNCTIONS_H
#define JSONHELPERFUNCTIONS_H

#include <QJsonDocument>
#include <QObject>

QByteArray createJsonFromDirectory(const QString &path);
QList<QVariantHash> fromJsonToHash(const QJsonDocument &jsonDoc);

#endif // JSONHELPERFUNCTIONS_H
