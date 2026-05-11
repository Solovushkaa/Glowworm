#include "json_utils.hpp"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "constants.hpp"

Q_LOGGING_CATEGORY(json_utils, "utils.json")

QByteArray createJsonFromDirectory(const QString &filePath)
{
    // All elements from the directory are selected
    // except the pointer to the current directory.
    QDir dir(filePath);
    QFileInfoList dirList = dir.entryInfoList(QDir::AllEntries | QDir::NoDot);

    QJsonArray jsonArray;
    QJsonObject jsonObject;

    for (auto &info : dirList) {
        jsonObject[constants::kName] = info.baseName();
        if(info.isDir()){
            jsonObject[constants::kIsDir] = true;
        } else {
            jsonObject[constants::kIsDir] = false;
        }
        jsonObject[constants::kSize] = info.size();
        jsonObject[constants::kPath] = info.absoluteFilePath();
        jsonObject[constants::kCreated] = info.birthTime().toString();
        jsonObject[constants::kModified] = info.lastModified().toString();
        jsonObject[constants::kAccessed] = info.lastRead().toString();
        jsonObject[constants::kIsReadable] = info.isReadable();

        jsonArray.append(jsonObject);
    }

    QJsonDocument jsonDoc(jsonArray);

    return jsonDoc.toJson();
}

QList<QVariantHash> fromJsonToHash(QByteArray &data)
{
    QList<QVariantHash> filesInfo;

    auto jsonDoc = QJsonDocument::fromJson(data);

    if(!jsonDoc.isArray()){
        qCWarning(json_utils) << "JSON Document isn't JSON Array";
    }

    QJsonArray jsonArray = jsonDoc.array();

    QJsonValue jsonValue;
    QJsonObject jsonObject;

    for(int i = 0; i < jsonArray.size(); ++i){
        jsonValue = jsonArray[i];

        if(jsonValue.isObject()){
            jsonObject = jsonValue.toObject();

            filesInfo.push_back(QVariantHash());
            filesInfo[i][constants::kName] = jsonObject[constants::kName].toString();
            filesInfo[i][constants::kIsDir] = jsonObject[constants::kIsDir].toBool();
            filesInfo[i][constants::kSize] = jsonObject[constants::kSize].toInteger();
            filesInfo[i][constants::kPath] = jsonObject[constants::kPath].toString();
            filesInfo[i][constants::kCreated] = jsonObject[constants::kCreated].toString();
            filesInfo[i][constants::kModified] = jsonObject[constants::kModified].toString();
            filesInfo[i][constants::kAccessed] = jsonObject[constants::kAccessed].toString();
            filesInfo[i][constants::kIsReadable] = jsonObject[constants::kIsReadable].toBool();
        }
    }
    return filesInfo;
}

QJsonObject parseJsonToObject(const QByteArray &jsonFileData)
{
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
