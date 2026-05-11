#ifndef MANAGERUTILS_HPP
#define MANAGERUTILS_HPP

#include <QJsonObject>
#include <QLoggingCategory>
#include <optional>

Q_DECLARE_LOGGING_CATEGORY(manager_utils)

std::optional<QByteArray> readAppDataFile(const QString &filePath);

bool rewriteAppDataFile(const QString &filePath, const QJsonObject &jsonDownloadInfo);

template<typename InfoType>
bool isCorrectAppDataKey(const QJsonObject &jsonObject);

template<typename Manager>
    requires std::is_class_v<Manager>
bool readAppData(Manager &manager, const QString &filePath, QJsonObject &jsonInfo);

#endif // MANAGERUTILS_HPP
