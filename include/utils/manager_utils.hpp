#ifndef MANAGERUTILS_HPP
#define MANAGERUTILS_HPP

#include <QJsonObject>
#include <optional>

std::optional<QByteArray> readConfigFile(const QString &filePath);

bool rewriteConfigFile(const QString &filePath, const QJsonObject &jsonDownloadInfo);

template<typename InfoType>
bool isCorrectAppDataKey(const QJsonObject &jsonObject);

template<typename Manager>
    requires std::is_class_v<Manager>
bool readPreset(Manager &manager, const QString &filePath, QJsonObject &jsonInfo);

#endif // MANAGERUTILS_HPP
