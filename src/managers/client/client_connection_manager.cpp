#include "client_connection_manager.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include "constants.hpp"
#include "manager_utils.hpp"

ClientConnectionManager::ClientConnectionManager(const QString &savePath)
    : m_savePath(savePath)
{}

bool ClientConnectionManager::readSavedConnections()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << R"(Error opening the "SavedConnections.json" file)";
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parsing error:" << parseError.errorString();
        qWarning() << "Error position:" << parseError.offset;
        return false;
    } else if (jsonDoc.isObject()) {
        if (m_jsonSavedConnection.isEmpty()) {
            emit connectionListUpdated();
            return true;
        }
    } else {
        qWarning() << "JSON is not an object";
        return false;
    }

    m_jsonSavedConnection = jsonDoc.object();

    QList<QString> deleteList;
    for (auto &key : m_jsonSavedConnection.keys()) {
        // --- Validation ---
        // if (!m_jsonSavedConnection[key].isObject()) {
        //     qWarning().nospace() << R"(Saved connection: ")" << key
        //                          << R"(" - has incorrect syntax)";
        //     deleteList.push_back(key);
        //     continue;
        // }

        const QJsonObject &jsonObject = m_jsonSavedConnection[key].toObject();
        m_savedConnections.push_back(ConnectionInfo());

        m_savedConnections.back().m_name = jsonObject[constants::kName].toString();
        if (jsonObject[constants::kProtocol].toString() == constants::DIRECT) {
            m_savedConnections.back().m_url.setHost(jsonObject[constants::kAddress].toString());
            m_savedConnections.back().m_url.port(jsonObject[constants::kPort].toInt());
        } else if (jsonObject[constants::kProtocol].toString() == constants::STUN) {
            // ----------------------------------------- //
        } else if (jsonObject[constants::kProtocol].toString() == constants::TURN) {
            // ----------------------------------------- //
        } else if (jsonObject[constants::kProtocol].toString() == constants::BLUETOOTH) {
            // ----------------------------------------- //
        }
    }
    qDebug() << "SavedConnections.json have been read";

    // Delete incorrect connections
    for (auto &key : deleteList) {
        m_jsonSavedConnection.remove(key);
    }

    emit connectionListUpdated();

    return true;
}

void ClientConnectionManager::setActive(int indx)
{
    m_activeConnection = &m_savedConnections[indx];
    emit activeConnectionsChanged();
}

void ClientConnectionManager::setConnectionInfoFromJson(ConnectionInfo &connectionInfo,
                                                        QJsonObject &jsonObject)
{}

void ClientConnectionManager::setJsonFromSavedConnections(QJsonObject &jsonObject,
                                                          ConnectionInfo &connectionInfo)
{
    jsonObject[constants::kTransport] = static_cast<int>(connectionInfo.m_transport);
    jsonObject[constants::kURL] = connectionInfo.m_url.toString();
    jsonObject[constants::kURL] = connectionInfo.m_bluetoothAddress.toString();
    jsonObject[constants::kURL] = connectionInfo.m_bluetoothUUID.toString();
    jsonObject[constants::kRemoteUserName] = connectionInfo.m_remoteUserName;
}

// void ClientConnectionManager::addDirect(const QString &name, const QString &address, int port)
// {
//     ConnectionInfo connInfo;
//     m_savedConnections.back().m_name = name;
//     m_savedConnections.back().m_url.setHost(address);
//     m_savedConnections.back().m_url.port(port);

//     QJsonObject jsonObject;
//     jsonObject[constants::kName] = name;
//     jsonObject[constants::kTransport] = std::move(constants::DIRECT.toString());
//     jsonObject[constants::kAddress] = address;
//     jsonObject[constants::kPort] = port;

//     addConnection(std::move(connInfo), std::move(jsonObject), name);
// }

template<typename ConnInfo>
    requires std::same_as<std::remove_reference_t<ConnInfo>, ConnectionInfo>
bool ClientConnectionManager::addConnection(ConnInfo &&connInfo)
{
    m_savedConnections.emplaceBack(std::move(connInfo));

    QJsonObject jsonObject;
    setJsonFromSavedConnections(jsonObject, m_savedConnections.back());
    m_jsonSavedConnection[m_savedConnections.back().m_name] = std::move(jsonObject);

    if constexpr (std::is_rvalue_reference_v<decltype(connInfo)>) {
        m_savedConnections.push_back(std::move(connInfo));
    } else {
        m_savedConnections.push_back(connInfo);
    }

    return rewriteConfigFile(m_savePath, m_jsonSavedConnection);
}
template bool ClientConnectionManager::addConnection<ConnectionInfo &&>(ConnectionInfo &&);
template bool ClientConnectionManager::addConnection<ConnectionInfo &>(ConnectionInfo &);

void ClientConnectionManager::remove(qint64 deleteIndex, qint64 activeIndex)
{
    if (activeIndex == deleteIndex) {
        m_activeConnection = nullptr;
    }

    m_jsonSavedConnection.remove(m_savedConnections[deleteIndex].m_name);
    m_savedConnections.remove(deleteIndex);

    rewriteConfigFile(m_savePath, m_jsonSavedConnection);

    emit connectionListUpdated();
}
