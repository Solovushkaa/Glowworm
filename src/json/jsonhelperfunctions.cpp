#include "jsonhelperfunctions.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "additional.h"

QByteArray createJsonFromDirectory(const QString &filePath)
{
    /* 
     * All elements from the directory are selected 
     * except the pointer to the current directory. 
     */
    QDir dir(filePath);
    QFileInfoList dirList = dir.entryInfoList(QDir::AllEntries | QDir::NoDot);

    QJsonArray jsonArray;
    QJsonObject jsonObject;

    /*
     * The necessary information about files from the 
     * current directory is selected for work.
     */
    using namespace constants;

    for (auto &info : dirList) {
        jsonObject[name] = info.baseName();
        if(info.isDir()){
            jsonObject[isDir] = true;
        } else {
            jsonObject[isDir] = false;
        }
        jsonObject[size] = info.size();
        jsonObject[path] = info.absoluteFilePath();
        jsonObject[created] = info.birthTime().toString();
        jsonObject[modified] = info.lastModified().toString();
        jsonObject[accessed] = info.lastRead().toString();
        jsonObject[isReadable] = info.isReadable();

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
        qWarning() << "JSON Document isn't JSON Array";
    }

    QJsonArray jsonArray = jsonDoc.array();

    QJsonValue jsonValue;
    QJsonObject jsonObject;

    /*
     * The necessary information about files from the 
     * current directory is selected for work.
     */
    using namespace constants;

    for(int i = 0; i < jsonArray.size(); ++i){
        jsonValue = jsonArray[i];

        if(jsonValue.isObject()){
            jsonObject = jsonValue.toObject();

            filesInfo.push_back(QVariantHash());
            filesInfo[i][name] = jsonObject[name].toString();
            filesInfo[i][isDir] = jsonObject[isDir].toBool();
            filesInfo[i][size] = jsonObject[size].toInteger();
            filesInfo[i][path] = jsonObject[path].toString();
            filesInfo[i][created] = jsonObject[created].toString();
            filesInfo[i][modified] = jsonObject[modified].toString();
            filesInfo[i][accessed] = jsonObject[accessed].toString();
            filesInfo[i][isReadable] = jsonObject[isReadable].toBool();
        }
    }
    return filesInfo;
}
