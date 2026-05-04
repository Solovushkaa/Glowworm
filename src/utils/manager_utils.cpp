#include "manager_utils.hpp"
#include <QFile>
#include "client_connection_manager.hpp"
#include "download_manager.hpp"
#include "json_utils.hpp"

std::optional<QByteArray> readConfigFile(const QString &filePath)
{
    QFile appDataFile(filePath);

    if (!appDataFile.open(QIODevice::ReadWrite)) {
        qCritical() << "Error opening the" << appDataFile.fileName()
                    << "file:" << appDataFile.errorString();
        return std::nullopt;
    }
    if (appDataFile.size() > 1073741824LL) {
        qCritical() << "The" << appDataFile.fileName() << "file is too large. Clean it up";
        return std::nullopt;
    }

    return appDataFile.readAll();
}

bool rewriteConfigFile(const QString &filePath, const QJsonObject &jsonDownloadInfo)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical().nospace() << "Error opening " << filePath << ": " << file.errorString();
        return false;
    }

    file.resize(0);
    file.write(QJsonDocument(jsonDownloadInfo).toJson());

    qInfo().nospace() << "Successful rewriting " << filePath;
    return true;
}

template<typename infoType>
bool isCorrectAppDataKey(const QJsonObject &jsonObject)
{
    constexpr bool is_downloadInfo = std::is_same_v<infoType, DownloadInfo>;

    auto enumMemberToString = [is_downloadInfo](const auto &info) -> QStringView {
        if constexpr (is_downloadInfo) {
            return getDownloadInfoMemberName(info);
        } else {
            return getConnectionInfoMemberName(info);
        }
    };

    using MemberClass = std::conditional_t<is_downloadInfo,
                                           downloads::DownloadInfoMember,
                                           connections::ConnectionInfoMember>;

    int size = static_cast<std::underlying_type_t<MemberClass>>(MemberClass::COUNT);
    QStringView memberName;
    for (int memberNum = 0; memberNum < size; ++memberNum) {
        memberName = enumMemberToString(static_cast<MemberClass>(memberNum));
        if (!jsonObject.contains(memberName)) {
            qWarning() << "The" << memberName << "key has an incorrect name";
            return false;
        }
    }

    return true;
}
template bool isCorrectAppDataKey<DownloadInfo>(const QJsonObject &jsonObject);
template bool isCorrectAppDataKey<ConnectionInfo>(const QJsonObject &jsonObject);

template<typename Manager>
    requires std::is_class_v<Manager>
bool readPreset(Manager &manager, const QString &filePath, QJsonObject &jsonInfo)
{
    auto fileData = readConfigFile(filePath);
    if (!fileData.has_value()) {
        return false;
    }

    jsonInfo = parseJsonToObject(fileData.value());

    using InfoType = typename Manager::InfoType;
    InfoType *info;

    QJsonObject jsonObject;
    for (auto &dictKey : jsonInfo.keys()) {
        if (!jsonInfo[dictKey].isObject()) {
            qWarning() << "An" << dictKey << "in json is not an JSON Object";
            return false;
        }

        // Validation
        jsonObject = jsonInfo[dictKey].toObject();
        if (!isCorrectAppDataKey<InfoType>(jsonObject)) {
            qWarning() << "Skip the JSON Object:" << dictKey;
            break;
        }

        info = new InfoType();
        //-------------------
        manager.initInfo(info, jsonObject);
        //-------------------
    }

    qInfo() << "The file" << filePath << "has been read";

    return rewriteConfigFile(filePath, jsonInfo);
}
template bool readPreset<DownloadManager>(DownloadManager &, const QString &, QJsonObject &);
template bool readPreset<ClientConnectionManager>(ClientConnectionManager &,
                                                  const QString &,
                                                  QJsonObject &);
