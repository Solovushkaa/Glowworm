#ifndef MANAGERUTILS_HPP
#define MANAGERUTILS_HPP

#include <QJsonObject>
#include <QLoggingCategory>
#include "file_info.hpp"
#include <optional>

Q_DECLARE_LOGGING_CATEGORY(manager_utils)

std::optional<QByteArray> readAppDataFile(const QString &filePath);

bool rewriteAppDataFile(const QString &filePath, const QJsonObject &jsonDownloadInfo);

template<typename InfoType>
bool isCorrectAppDataKey(const QJsonObject &jsonObject);

template<typename Manager>
    requires std::is_class_v<Manager>
bool readAppData(Manager &manager, const QString &filePath, QJsonObject &jsonInfo);

/**
 * @brief fromJsonToFileInfo
 * 
 * Creates an array of directory files.
 * 
 * @param data JSON object converted to a ByteArray
 * @return Array of directory files
 */
std::vector<std::unique_ptr<FileInfo>> fromJsonToFileInfo(QByteArray &data);

#endif // MANAGERUTILS_HPP
