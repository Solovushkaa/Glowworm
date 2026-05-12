/**
 * @file json_utils.hpp
 * @brief A set of functions for working with JSON.
 *
 */

#ifndef JSONUTILS_HPP
#define JSONUTILS_HPP

#include <QJsonObject>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(json_utils)

/**
 * @brief createJsonFromDirectory
 * 
 * Creates a JSON file from the directory located at the specified path.
 * 
 * @param path Path to the directory
 * @return JSON object converted to a ByteArray
 */
QByteArray createJsonFromDirectory(const QString &dirPath);

QJsonObject parseJsonToObject(const QByteArray &data);

#endif // JSONUTILS_HPP
