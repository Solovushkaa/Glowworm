#ifndef MANAGERUTILS_HPP
#define MANAGERUTILS_HPP

#include <QJsonObject>

bool rewriteConfigFile(const QString &filePath, const QJsonObject &jsonDownloadInfo);

#endif // MANAGERUTILS_HPP
