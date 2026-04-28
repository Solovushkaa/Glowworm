#include "download_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

DownloadManager::DownloadManager(const QString &savePath)
    : m_savePath(savePath)
{}

// TODO: Think about refactoring and behavior when reading the broken config
bool DownloadManager::readUnfinishedDownloads()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << R"(Error opening the "UnfinishedDownloads" file)";
        return false;
    }
    if (file.size() > 1073741824LL) {
        qWarning() << R"(The "UnfinishedDownloads" file is too large. Clean it up)";
        return false;
    }

    QByteArray data = std::move(file.readAll());

    m_jsonDownloadInfo = parseJson(data).object();
    if (m_jsonDownloadInfo.size() == 0) {
        qDebug() << "Successful data reading";
        return true;
    }

    DownloadInfo downloadInfo;
    for (auto &dictKey : m_jsonDownloadInfo.keys()) {
        if (!m_jsonDownloadInfo[dictKey].isObject()) {
            qWarning().nospace() << R"(An ")" << dictKey << R"(" in json is not an jsonObject)";
            return false;
        }

        QJsonObject jsonObject = m_jsonDownloadInfo[dictKey].toObject();
        if (!isCorrectDownloadInfoObject(jsonObject)) {
            qWarning() << "Skip the jsonObject";
            break;
        }

        setDownloadInfoFromJson(downloadInfo, jsonObject);

        m_downloadInfoDict.emplace(downloadInfo.m_downloadID, std::move(downloadInfo));

        // To display the current State correctly
        jsonObject[constants::kDownloadState] = static_cast<int>(DownloadState::Pause);
    }

    file.close();
    return rewriteConfigFile(m_savePath, m_jsonDownloadInfo);
}

bool DownloadManager::isCorrectDownloadInfoObject(const QJsonObject &jsonObject)
{
    using enum DownloadInfoMember;
    auto diToString = getDownloadInfoMemberName;

    int size = static_cast<std::underlying_type_t<DownloadInfoMember>>(COUNT);
    QStringView memberName;
    for (int memberNum = 0; memberNum < size; ++memberNum) {
        memberName = diToString(static_cast<DownloadInfoMember>(memberNum));
        if (jsonObject.find(memberName) == jsonObject.end()) {
            qWarning().nospace().noquote()
                << R"(The ")" << memberName << R"(" jsonObject has an incorrect name)";
            return false;
        }
    }

    return true;
}

void DownloadManager::setDownloadInfoFromJson(DownloadInfo &downloadInfo, QJsonObject &jsonObject)
{
    downloadInfo.m_downloadID = jsonObject[constants::kDownloadID].toString();
    downloadInfo.m_hostKey = jsonObject[constants::kHostkey].toString();
    downloadInfo.m_URL = jsonObject[constants::kURL].toString();
    downloadInfo.m_name = jsonObject[constants::kName].toString();
    downloadInfo.m_path = jsonObject[constants::kPath].toString();
    downloadInfo.m_saveName = jsonObject[constants::kSaveName].toString();
    downloadInfo.m_savePath = jsonObject[constants::kSavePath].toString();
    downloadInfo.m_size = jsonObject[constants::kSize].toInteger();
    downloadInfo.m_lastReceivedByte = jsonObject[constants::kLastReceivedByte].toInteger();
    downloadInfo.m_created = jsonObject[constants::kCreated].toString();
    downloadInfo.m_modified = jsonObject[constants::kModified].toString();
    downloadInfo.m_accessed = jsonObject[constants::kAccessed].toString();
    downloadInfo.m_downloadState = DownloadState::Pause;
}

void DownloadManager::setJsonFromDownloadInfo(QJsonObject &jsonObject, DownloadInfo &downloadInfo)
{
    jsonObject[constants::kDownloadID] = downloadInfo.m_downloadID;
    jsonObject[constants::kURL] = downloadInfo.m_URL.toString();
    jsonObject[constants::kHostkey] = downloadInfo.m_hostKey;
    jsonObject[constants::kName] = downloadInfo.m_name;
    jsonObject[constants::kPath] = downloadInfo.m_path;
    jsonObject[constants::kSaveName] = downloadInfo.m_saveName;
    jsonObject[constants::kSavePath] = downloadInfo.m_savePath;
    jsonObject[constants::kSize] = downloadInfo.m_size;
    jsonObject[constants::kLastReceivedByte] = downloadInfo.m_lastReceivedByte;
    jsonObject[constants::kCreated] = downloadInfo.m_created;
    jsonObject[constants::kModified] = downloadInfo.m_modified;
    jsonObject[constants::kAccessed] = downloadInfo.m_accessed;
    jsonObject[constants::kDownloadState] = static_cast<int>(downloadInfo.getDownloadState());
}

QJsonDocument DownloadManager::parseJson(QByteArray &data)
{
    QJsonDocument unfinishedDownloadInfo;
    if (data.size() < 100) {
        unfinishedDownloadInfo = QJsonDocument(QJsonObject());
        return unfinishedDownloadInfo;
    }
    QJsonParseError parseError;
    unfinishedDownloadInfo = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parsing error:\n" << parseError.errorString();
        qWarning() << "Error position:" << parseError.offset;

        unfinishedDownloadInfo = QJsonDocument(QJsonObject());
    } else if (!unfinishedDownloadInfo.isObject()) {
        qWarning() << "JSON is not an object";
        unfinishedDownloadInfo = QJsonDocument(QJsonObject());
    }

    return unfinishedDownloadInfo;
}

template<typename DInfo>
    requires std::same_as<std::remove_reference_t<DInfo>, DownloadInfo>
bool DownloadManager::addDownload(DInfo &&downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QJsonObject jsonObject = m_jsonDownloadInfo[name].toObject();

    setJsonFromDownloadInfo(jsonObject, downloadInfo);

    if constexpr (std::is_rvalue_reference_v<decltype(downloadInfo)>) {
        m_downloadInfoDict[downloadInfo.m_downloadID] = std::move(downloadInfo);
    } else {
        m_downloadInfoDict[downloadInfo.m_downloadID] = downloadInfo;
    }

    return rewriteConfigFile(m_savePath, m_jsonDownloadInfo);
}
template bool DownloadManager::addDownload<DownloadInfo &>(DownloadInfo &);
template bool DownloadManager::addDownload<DownloadInfo &&>(DownloadInfo &&);

bool DownloadManager::deleteDownload(const DownloadInfo &downloadInfo)
{
    QString deleteID = std::move(downloadInfo.m_downloadID);

    m_downloadInfoDict.remove(deleteID);
    m_jsonDownloadInfo.remove(deleteID);

    return rewriteConfigFile(m_savePath, m_jsonDownloadInfo);
}

QList<QVariantHash> DownloadManager::getVariantListUnfinishedDownloads()
{
    QList<QVariantHash> unfinishedDownloads;
    for (auto &downloadInfo : m_downloadInfoDict) {
        unfinishedDownloads.push_back(QVariantHash());
        unfinishedDownloads.back()[constants::kName] = downloadInfo.m_name;
        unfinishedDownloads.back()[constants::kDownloadID] = downloadInfo.m_downloadID;
        unfinishedDownloads.back()[constants::kSize] = downloadInfo.m_size;
        unfinishedDownloads.back()[constants::kLastReceivedByte] = downloadInfo.m_lastReceivedByte;
    }

    return unfinishedDownloads;
}
