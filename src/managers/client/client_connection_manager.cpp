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
    return readPreset(*this, m_savePath, m_jsonSavedConnections);
}

void ClientConnectionManager::initInfo(ConnectionInfo &connectionInfo, QJsonObject &jsonObject)
{
    setConnectionInfoFromJson(connectionInfo, jsonObject);

    m_savedConnections.push_back(connectionInfo);
}

void ClientConnectionManager::setActive(int indx)
{
    m_activeConnection = &m_savedConnections[indx];
    emit activeConnectionsChanged();
}

void ClientConnectionManager::setConnectionInfoFromJson(ConnectionInfo &connectionInfo,
                                                        QJsonObject &jsonObject)
{
    connectionInfo.m_name = jsonObject[constants::kName].toString();
    connectionInfo.m_transport = static_cast<Transport>(jsonObject[constants::kTransport].toInt());

    connectionInfo.m_url.setHost(jsonObject[constants::kAddress].toString());
    connectionInfo.m_url.port(jsonObject[constants::kPort].toInt());

    if (!jsonObject[constants::kRemoteUserName].isNull()) {
        connectionInfo.m_remoteUserName = jsonObject[constants::kRemoteUserName].toString();
    }
    if (!jsonObject[constants::kBluetoothAddress].isNull()) {
        connectionInfo.m_bluetoothAddress = QBluetoothAddress(
            jsonObject[constants::kBluetoothAddress].toString());
    }
    if (!jsonObject[constants::kBluetoothUUID].isNull()) {
        connectionInfo.m_bluetoothUUID = QBluetoothUuid(
            jsonObject[constants::kBluetoothUUID].toString());
    }
}

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
//     ConnectionInfo connectionInfo;
//     m_savedConnections.back().m_name = name;
//     m_savedConnections.back().m_url.setHost(address);
//     m_savedConnections.back().m_url.port(port);

//     QJsonObject jsonObject;
//     jsonObject[constants::kName] = name;
//     jsonObject[constants::kTransport] = std::move(constants::DIRECT.toString());
//     jsonObject[constants::kAddress] = address;
//     jsonObject[constants::kPort] = port;

//     addConnection(std::move(connectionInfo), std::move(jsonObject), name);
// }

template<typename ConnInfo>
    requires std::same_as<std::remove_reference_t<ConnInfo>, ConnectionInfo>
bool ClientConnectionManager::addConnection(ConnInfo &&connectionInfo)
{
    QJsonObject jsonObject;
    setJsonFromSavedConnections(jsonObject, m_savedConnections.back());

    m_jsonSavedConnections.insert(m_savedConnections.back().m_name, std::move(jsonObject));

    if constexpr (std::is_rvalue_reference_v<decltype(connectionInfo)>) {
        m_savedConnections.push_back(std::move(connectionInfo));
    } else {
        m_savedConnections.push_back(connectionInfo);
    }

    return rewriteConfigFile(m_savePath, m_jsonSavedConnections);
}
template bool ClientConnectionManager::addConnection<ConnectionInfo &&>(ConnectionInfo &&);
template bool ClientConnectionManager::addConnection<ConnectionInfo &>(ConnectionInfo &);

bool ClientConnectionManager::deleteConnection(qint64 deleteIndex, qint64 activeIndex)
{
    if (activeIndex == deleteIndex) {
        m_activeConnection = nullptr;
    }

    m_jsonSavedConnections.remove(m_savedConnections[deleteIndex].m_name);
    m_savedConnections.remove(deleteIndex);

    return rewriteConfigFile(m_savePath, m_jsonSavedConnections);
}
