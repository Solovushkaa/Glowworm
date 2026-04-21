#include "downloadmanager.hpp"
#include <QFile>

DownloadManager::DownloadManager()
{
    if (readUnfinishedDownloads()) {
        qDebug() << "Incomplete downloads have been successfully read";
    } else {
        qWarning() << "Error reading incomplete downloads";
    }
}

bool DownloadManager::readUnfinishedDownloads()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << R"(Error opening the "UnfinishedDownloads" file)";
        return false;
    }

    QJsonParseError parseError;
    if (file.size() > 1073741824LL) {
        // file.resize(0); //!!!
        return false;
    }
    QJsonDocument unfinishedDownloadInfo(QJsonDocument::fromJson(file.readAll(), &parseError));

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parsing error:" << parseError.errorString();
        qWarning() << "Error position:" << parseError.offset;
        return false;
    } else if (unfinishedDownloadInfo.isObject()) {
        m_jsonDownloadInfo = unfinishedDownloadInfo.object();
    } else {
        qWarning() << "JSON is not an object";
        return false;
    }

    DownloadInfo downloadInfo;
    for (auto &dictKey : m_jsonDownloadInfo.keys()) {
        if (!m_jsonDownloadInfo[dictKey].isObject()) {
            qWarning().nospace() << R"(An ")" << dictKey << R"(" in json is not an object)";
            return false;
        }

        QJsonObject object = m_jsonDownloadInfo[dictKey].toObject();
        if (isCorrectDownloadInfoObject(object)) {
            qWarning() << "Skip the object";
            break;
        }

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

        m_downloadInfoDict.emplace(downloadInfo.m_downloadID, std::move(downloadInfo));

        object[constants::DOWNLOADSTATUS] = static_cast<int>(State::Pause);
    }

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    return true;
}

bool DownloadManager::isCorrectDownloadInfoObject(const QJsonObject &object)
{
    // Псевдоним для упрощённого ввода
    using enum DownloadInfoMember;
    auto diToString = getDownloadInfoMemberName;

    int size = static_cast<std::underlying_type_t<DownloadInfoMember>>(COUNT);
    const char *memberName;
    for (int memberNum = 0; memberNum < size; ++memberNum) {
        memberName = diToString(static_cast<DownloadInfoMember>(memberNum));
        if (object.find(memberName) == object.end()) {
            qWarning().nospace() << R"(The ")" << memberName << R"(" object has an incorrect name)";
            return false;
        }
    }

    return true;
}

bool DownloadManager::addDownloadToUnfinished(DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug().nospace() << R"(Error opening the "UnfinishedDownloads" file to add ")" << name
                           << R"(" download)";
        return false;
    }

    const QJsonObject &object = m_jsonDownloadInfo[name].toObject();
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

    m_downloadInfoDict[name] = std::move(downloadInfo);

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    qDebug() << R"(Added a new ")" << name << R"(" download)";

    return true;
}

bool DownloadManager::deleteFromUnfinishedDownload(const DownloadInfo &downloadInfo)
{
    QString name(downloadInfo.m_hostKey + "_hostKey_" + downloadInfo.m_downloadID);

    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug().nospace() << R"(Error opening the "UnfinishedDownloads" file to delete ")" << name
                           << R"(" download)";
        return false;
    }

    m_downloadInfoDict.remove(name);
    m_jsonDownloadInfo.remove(name);

    file.resize(0);
    file.write(QJsonDocument(m_jsonDownloadInfo).toJson());

    qDebug().nospace() << R"(The download ")" << name
                       << R"(" is finished and removed from the list)";

    return true;
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
