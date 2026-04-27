#include "downloadmanager.hpp"

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
            qWarning().nospace() << R"(An ")" << dictKey << R"(" in json is not an object)";
            return false;
        }

        QJsonObject object = m_jsonDownloadInfo[dictKey].toObject();
        if (!isCorrectDownloadInfoObject(object)) {
            qWarning() << "Skip the object";
            break;
        }

        setDownloadInfoFromJson(downloadInfo, object);

        m_downloadInfoDict.emplace(downloadInfo.m_downloadID, std::move(downloadInfo));

        // To display the current State correctly
        object[constants::DOWNLOADSTATUS] = static_cast<int>(State::Pause);
    }

    file.close();
    return rewriteFile(uR"(Error opening the "UnfinishedDownloads" file)",
                       u"Successful data reading");
}

bool DownloadManager::isCorrectDownloadInfoObject(const QJsonObject &object)
{
    using enum DownloadInfoMember;
    auto diToString = getDownloadInfoMemberName;

    int size = static_cast<std::underlying_type_t<DownloadInfoMember>>(COUNT);
    QStringView memberName;
    for (int memberNum = 0; memberNum < size; ++memberNum) {
        memberName = diToString(static_cast<DownloadInfoMember>(memberNum));
        if (object.find(memberName) == object.end()) {
            qWarning().nospace().noquote()
                << R"(The ")" << memberName << R"(" object has an incorrect name)";
            return false;
        }
    }

    return true;
}

bool DownloadManager::rewriteFile(QStringView errorString, QStringView successString)
{
    QFile file(m_savePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << errorString;
        return false;
    }

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    qDebug() << successString;
    return true;
}

void DownloadManager::setDownloadInfoFromJson(DownloadInfo &downloadInfo, QJsonObject &object)
{
    downloadInfo.m_downloadID = object[constants::DOWNLOADID].toString();
    downloadInfo.m_hostKey = object[constants::HOSTKEY].toString();
    downloadInfo.m_URL = object[constants::URL].toString();
    downloadInfo.m_name = object[constants::NAME].toString();
    downloadInfo.m_path = object[constants::PATH].toString();
    downloadInfo.m_saveName = object[constants::SAVENAME].toString();
    downloadInfo.m_savePath = object[constants::SAVEPATH].toString();
    downloadInfo.m_size = object[constants::SIZE].toInteger();
    downloadInfo.m_lastReceivedByte = object[constants::LASTRECEIVEDBYTE].toInteger();
    downloadInfo.m_created = object[constants::CREATED].toString();
    downloadInfo.m_modified = object[constants::MODIFIED].toString();
    downloadInfo.m_accessed = object[constants::ACCESSED].toString();
    downloadInfo.m_downloadStatus = State::Pause;
}

void DownloadManager::setJsonFromDownloadInfo(QJsonObject &object, DownloadInfo &downloadInfo)
{
    object[constants::DOWNLOADID] = downloadInfo.m_downloadID;
    object[constants::URL] = downloadInfo.m_URL.toString();
    object[constants::HOSTKEY] = downloadInfo.m_hostKey;
    object[constants::NAME] = downloadInfo.m_name;
    object[constants::PATH] = downloadInfo.m_path;
    object[constants::SAVENAME] = downloadInfo.m_saveName;
    object[constants::SAVEPATH] = downloadInfo.m_savePath;
    object[constants::SIZE] = downloadInfo.m_size;
    object[constants::LASTRECEIVEDBYTE] = downloadInfo.m_lastReceivedByte;
    object[constants::CREATED] = downloadInfo.m_created;
    object[constants::MODIFIED] = downloadInfo.m_modified;
    object[constants::ACCESSED] = downloadInfo.m_accessed;
    object[constants::DOWNLOADSTATUS] = static_cast<int>(downloadInfo.getDownloadStatus());
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
bool DownloadManager::addDownloadToUnfinished(DInfo &&downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QJsonObject object = m_jsonDownloadInfo[name].toObject();

    setJsonFromDownloadInfo(object, downloadInfo);

    if constexpr (std::is_rvalue_reference_v<decltype(downloadInfo)>) {
        m_downloadInfoDict[downloadInfo.m_downloadID] = std::move(downloadInfo);
    } else {
        m_downloadInfoDict[downloadInfo.m_downloadID] = downloadInfo;
    }

    return rewriteFile(R"(Error opening the "UnfinishedDownloads" file to add ")" + name
                           + R"(" download)",
                       R"(Added a new ")" + name + R"(" download)");
}
template bool DownloadManager::addDownloadToUnfinished<DownloadInfo &>(DownloadInfo &);
template bool DownloadManager::addDownloadToUnfinished<DownloadInfo &&>(DownloadInfo &&);

bool DownloadManager::deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo)
{
    // QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);
    QString deleteID = std::move(downloadInfo.m_downloadID);

    m_downloadInfoDict.remove(deleteID);
    m_jsonDownloadInfo.remove(deleteID);

    return rewriteFile(R"(Error opening the "UnfinishedDownloads" file to delete ")" + deleteID
                           + R"(" download)",
                       R"(The download ")" + deleteID
                           + R"(" is finished and removed from the list)");
}

QList<QVariantHash> DownloadManager::getVariantListUnfinishedDownloads()
{
    QList<QVariantHash> unfinishedDownloads;
    for (auto &downloadInfo : m_downloadInfoDict) {
        unfinishedDownloads.push_back(QVariantHash());
        unfinishedDownloads.back()[constants::NAME] = downloadInfo.m_name;
        unfinishedDownloads.back()[constants::DOWNLOADID] = downloadInfo.m_downloadID;
        unfinishedDownloads.back()[constants::SIZE] = downloadInfo.m_size;
        unfinishedDownloads.back()[constants::LASTRECEIVEDBYTE] = downloadInfo.m_lastReceivedByte;
    }

    return unfinishedDownloads;
}

// void DownloadManager::onDownloadAdded(QString &saveName,
//                                                 QString &downloadID,
//                                                 QVariantHash &refFileInfo)
// {
//     DownloadInfo downloadInfo(downloadID,
//                               m_url,
//                               m_currentHostKey,
//                               refFileInfo["name"].toString(),
//                               refFileInfo["path"].toString(),
//                               saveName,
//                               savePath,
//                               refFileInfo["size"].toLongLong(),
//                               0,
//                               refFileInfo["created"].toString(),
//                               refFileInfo["modified"].toString(),
//                               refFileInfo["accessed"].toString(),
//                               State::Active);

//     addDownloadToUnfinished(downloadInfo);

//     m_downloadInfoDict.emplace(downloadID, std::move(downloadInfo));
// }
