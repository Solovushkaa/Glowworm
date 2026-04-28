/**
 * @file json_utils.hpp
 * @brief A set of functions for working with JSON.
 *
 */

#ifndef JSONUTILS_HPP
#define JSONUTILS_HPP

#include <QVariantHash>

/**
 * @brief createJsonFromDirectory
 * 
 * Creates a JSON file from the directory located at the specified path.
 * 
 * @param path Path to the directory
 * @return JSON object converted to a ByteArray
 */
QByteArray createJsonFromDirectory(const QString &path);

/**
 * @brief fromJsonToHash
 * 
 * Creates an array of directory files.
 * 
 * @param data JSON object converted to a ByteArray
 * @return Array of directory files
 */
QList<QVariantHash> fromJsonToHash(QByteArray &data);

#endif // JSONUTILS_HPP
