#include "json_utils.hpp"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "constants.hpp"

Q_LOGGING_CATEGORY(json_utils, "utils.json")

QJsonArray createJsonFromDirectory(const QString &dirPath)
{
    qCDebug(json_utils) << "Generating JSON from the directory at the path:" << dirPath;

    // All elements from the directory are selected
    // except the pointer to the current directory.
    QDir dir(dirPath);
    QFileInfoList dirList = dir.entryInfoList(QDir::AllEntries | QDir::NoDot);

    QJsonArray jsonArray;
    QJsonObject jsonObject;

    for (auto &info : dirList) {
        jsonObject[constants::kName] = info.fileName();
        jsonObject[constants::kPath] = info.absoluteFilePath();
        jsonObject[constants::kCreated] = info.birthTime().toString();
        jsonObject[constants::kModified] = info.lastModified().toString();
        jsonObject[constants::kAccessed] = info.lastRead().toString();
        jsonObject[constants::kSize] = info.size();
        if (info.isDir()) {
            jsonObject[constants::kIsDir] = true;
        } else {
            jsonObject[constants::kIsDir] = false;
        }
        jsonObject[constants::kIsReadable] = info.isReadable();

        jsonArray.append(jsonObject);
    }

    return jsonArray;
}

QJsonObject parseJsonToObject(const QByteArray &jsonFileData)
{
    qCDebug(json_utils) << "Parsing a JSON document into a JSON object";

    QJsonDocument appDataJsonDoc;

    QJsonParseError parseError;
    appDataJsonDoc = QJsonDocument::fromJson(jsonFileData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qCWarning(json_utils) << "JSON parsing error:\n" << parseError.errorString();
        qCWarning(json_utils) << "Error position:" << parseError.offset;

        appDataJsonDoc = QJsonDocument(QJsonObject());
    } else if (!appDataJsonDoc.isObject()) {
        qCWarning(json_utils) << "JSON is not an object";
        appDataJsonDoc = QJsonDocument(QJsonObject());
    }

    return appDataJsonDoc.object();
}
