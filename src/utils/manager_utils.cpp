#include "manager_utils.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include "client_connection_manager.hpp"
#include "constants.hpp"
#include "download_manager.hpp"
#include "json_utils.hpp"

Q_LOGGING_CATEGORY(manager_utils, "utils.managers")

std::optional<QByteArray> readAppDataFile(const QString &filePath)
{
    qCDebug(manager_utils) << "Reading application data from:" << filePath;

    QFile appDataFile(filePath);

    if (!appDataFile.open(QIODevice::ReadWrite)) {
        qCCritical(manager_utils) << "Error opening the" << appDataFile.fileName()
                                  << "file:" << appDataFile.errorString();
        return std::nullopt;
    }
    if (appDataFile.size() > 1073741824LL) {
        qCCritical(manager_utils) << "The" << appDataFile.fileName()
                                  << "file is too large. Clean it up";
        return std::nullopt;
    }

    return appDataFile.readAll();
}

bool rewriteAppDataFile(const QString &filePath, const QJsonObject &jsonDownloadInfo)
{
    qCDebug(manager_utils) << "Overwriting application data in:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCCritical(manager_utils).nospace()
            << "Error opening " << filePath << ": " << file.errorString();
        return false;
    }

    file.resize(0);
    file.write(QJsonDocument(jsonDownloadInfo).toJson());

    qCInfo(manager_utils) << "Successful rewriting:" << filePath;
    return true;
}

template<typename InfoType>
bool isCorrectAppDataKey(const QJsonObject &jsonObject)
{
    qCDebug(manager_utils) << "Validating a JSON object";

    constexpr bool is_downloadInfo = std::is_same_v<InfoType, DownloadInfo>;

    auto enumMemberToString = [](const auto &info) -> QStringView {
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
            qCWarning(manager_utils) << "The" << memberName << "key has an incorrect name";
            return false;
        }
    }

    return true;
}
template bool isCorrectAppDataKey<DownloadInfo>(const QJsonObject &jsonObject);
template bool isCorrectAppDataKey<ConnectionInfo>(const QJsonObject &jsonObject);

template<typename Manager>
    requires std::is_class_v<Manager>
bool readAppData(Manager &manager, const QString &filePath, QJsonObject &jsonInfo)
{
    using InfoType = typename Manager::InfoType;
    constexpr bool isDownloadInfoType = std::is_same_v<InfoType, DownloadManager::InfoType>;
    if constexpr (isDownloadInfoType) {
        qCDebug(manager_utils) << "Reading application data for DownloadManager";
    } else {
        qCDebug(manager_utils) << "Reading application data for ClientConnectionManager";
    }

    auto fileData = readAppDataFile(filePath);
    if (!fileData.has_value()) {
        return false;
    }

    jsonInfo = parseJsonToObject(fileData.value());

    InfoType *info;

    QJsonObject jsonObject;
    for (auto &dictKey : jsonInfo.keys()) {
        if (!jsonInfo[dictKey].isObject()) {
            qCWarning(manager_utils) << "An" << dictKey << "in json is not an JSON Object";
            return false;
        }

        // Validation
        jsonObject = jsonInfo[dictKey].toObject();
        if (!isCorrectAppDataKey<InfoType>(jsonObject)) {
            qCWarning(manager_utils) << "Skip the JSON Object:" << dictKey;
            jsonInfo.remove(dictKey);
            break;
        }
        if constexpr (isDownloadInfoType) {
            if (static_cast<DownloadInfo::DownloadState>(
                    jsonObject[constants::kDownloadState].toInt())
                == DownloadInfo::DownloadState::Finish) {
                jsonInfo.remove(dictKey);
                break;
            }
        }

        info = new InfoType();
        //-------------------
        manager.initInfo(info, jsonObject);
        //-------------------
    }

    qCInfo(manager_utils) << "The file" << filePath << "has been read";

    return rewriteAppDataFile(filePath, jsonInfo);
}
template bool readAppData<DownloadManager>(DownloadManager &, const QString &, QJsonObject &);
template bool readAppData<ClientConnectionManager>(ClientConnectionManager &,
                                                   const QString &,
                                                   QJsonObject &);

std::vector<std::unique_ptr<FileInfo>> fromJsonToFileInfo(const QJsonArray &directoryJsonArray)
{
    qCDebug(manager_utils) << "Generating FileInfo from the JSON";

    std::vector<std::unique_ptr<FileInfo>> filesInfo;

    QJsonObject jsonObject;
    for (auto jsonValue : directoryJsonArray) {
        if (jsonValue.isObject()) {
            jsonObject = jsonValue.toObject();

            filesInfo.push_back(
                std::make_unique<FileInfo>(jsonObject[constants::kName].toString(),
                                           jsonObject[constants::kPath].toString(),
                                           jsonObject[constants::kCreated].toString(),
                                           jsonObject[constants::kModified].toString(),
                                           jsonObject[constants::kAccessed].toString(),
                                           jsonObject[constants::kSize].toInteger(),
                                           jsonObject[constants::kIsDir].toBool(),
                                           jsonObject[constants::kIsReadable].toBool()));
        } else {
            return {};
        }
    }
    return filesInfo;
}
