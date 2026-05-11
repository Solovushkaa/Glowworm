#include "client_connection_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(connection_manager, "connection.manager")

ClientConnectionManager::ClientConnectionManager(const QString &savePath, QObject *parent)
    : m_savePath(savePath)
    , QAbstractListModel(parent)
{
    qCDebug(connection_manager) << "ClientConnectionManager successfully created";
}

ClientConnectionManager::~ClientConnectionManager()
{
    qCDebug(connection_manager) << "ClientConnectionManager successfully destroyed";
}

int ClientConnectionManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        qCDebug(connection_manager) << "QModelIndex parent is not valid";
        return 0;
    return m_connectionInfoList.size();
}

QVariant ClientConnectionManager::data(const QModelIndex &index, int role) const
{
    qCDebug(connection_manager) << "Getting ClientConnectionManager data for the QML List Model";

    if (!index.isValid() || index.row() >= m_connectionInfoList.size())
        return QVariant();

    ConnectionInfo *info = m_connectionInfoList.at(index.row());
    switch (role) {
    case NameRole:
        return info->m_name;
    case TransportRole:
        return static_cast<int>(info->m_transport);
    case URLRole:
        return info->m_url;
    case RemoteUserRole:
        return info->m_remoteUserName;
    case BluetoothAddressRole:
        return info->m_bluetoothAddress.toString();
    case BluetoothUUIDRole:
        return info->m_bluetoothUUID.toString();
    case StateRole:
        return static_cast<int>(info->m_connectionState);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ClientConnectionManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = constants::kName.toUtf8();
    roles[TransportRole] = constants::kTransport.toUtf8();
    roles[URLRole] = constants::kURL.toUtf8();
    roles[RemoteUserRole] = constants::kRemoteUserName.toUtf8();
    roles[BluetoothAddressRole] = constants::kBluetoothAddress.toUtf8();
    roles[BluetoothUUIDRole] = constants::kBluetoothUUID.toUtf8();
    roles[StateRole] = constants::kConnectionState.toUtf8();
    return roles;
}

bool ClientConnectionManager::addConnection(ConnectionInfo *connectionInfo)
{
    const int row = m_connectionInfoList.size();
    beginInsertRows(QModelIndex(), row, row);
    m_connectionInfoList.append(connectionInfo);
    endInsertRows();

    QJsonObject jsonObject;
    setJsonObjectFromConnectionInfo(jsonObject, connectionInfo);

    m_jsonSavedConnections.insert(connectionInfo->m_name, std::move(jsonObject));
    m_connectionInfoDict[connectionInfo->m_name] = connectionInfo;

    emit connectionAdded();

    return rewriteAppDataFile(m_savePath, m_jsonSavedConnections);
}

bool ClientConnectionManager::addConnection(const QString &name,
                                            ConnectionInfo::Transport transport,
                                            const QUrl &url,
                                            const QString &remoteUserName,
                                            const QBluetoothAddress &bluetoothAddress,
                                            const QBluetoothUuid &bluetoothUUID)
{
    qCDebug(connection_manager) << "Adding a new connection";

    if (m_connectionInfoDict.contains(name)) {
        qCInfo(connection_manager) << "A connection with the name" << name << "already exists";
        return false;
    }

    auto connectionInfo = new ConnectionInfo(name,
                                             transport,
                                             url,
                                             remoteUserName,
                                             bluetoothAddress,
                                             bluetoothUUID,
                                             ConnectionInfo::ConnectionState::Disconnected,
                                             this);

    return addConnection(connectionInfo);
}

bool ClientConnectionManager::deleteConnection(int activeIndex, int deleteIndex)
{
    qCDebug(connection_manager) << "Deleting a connection";

    if (deleteIndex < 0 || deleteIndex >= m_connectionInfoList.size()) {
        qCCritical(connection_manager) << "Bad ConnectionInfo delete index!";
        return false;
    }

    if (activeIndex == deleteIndex) {
        m_activeConnection = nullptr;
    }

    QString deleteID = std::move(m_connectionInfoList[deleteIndex]->m_name);

    beginRemoveRows(QModelIndex(), deleteIndex, deleteIndex);
    ConnectionInfo *info = m_connectionInfoList.takeAt(deleteIndex);
    info->deleteLater();
    endRemoveRows();

    m_connectionInfoDict.remove(deleteID);
    m_jsonSavedConnections.remove(deleteID);

    emit connectionRemoved(deleteIndex);

    return rewriteAppDataFile(m_savePath, m_jsonSavedConnections);
}

bool ClientConnectionManager::updateConnection(int index,
                                               const QString &property,
                                               const QVariant &value)
{
    qCDebug(connection_manager) << "Updating ClientConnectioManager data for the QML List Model";

    if (index < 0 || index >= m_connectionInfoList.size()) {
        qCCritical(connection_manager) << "Bad ConnectionInfo update index!";
        return false;
    }

    ConnectionInfo *info = m_connectionInfoList[index];

    info->setProperty(property.toUtf8().constData(), value);

    QVector<int> roles;
    if (property == constants::kName.toUtf8())
        roles << NameRole;
    if (property == constants::kTransport.toUtf8())
        roles << TransportRole;
    if (property == constants::kURL.toUtf8())
        roles << URLRole;
    if (property == constants::kRemoteUserName.toUtf8())
        roles << RemoteUserRole;
    if (property == constants::kBluetoothAddress.toUtf8())
        roles << BluetoothAddressRole;
    if (property == constants::kBluetoothUUID.toUtf8())
        roles << BluetoothUUIDRole;
    if (property == constants::kConnectionState.toUtf8())
        roles << StateRole;

    static const QVector<int> allRoles{NameRole,
                                       TransportRole,
                                       URLRole,
                                       RemoteUserRole,
                                       BluetoothAddressRole,
                                       BluetoothUUIDRole,
                                       StateRole};

    QModelIndex idx = this->index(index);

    emit dataChanged(idx, idx, roles.isEmpty() ? allRoles : roles);

    return true;
}

bool ClientConnectionManager::readSavedConnections()
{
    return readAppData(*this, m_savePath, m_jsonSavedConnections);
}

void ClientConnectionManager::initInfo(ConnectionInfo *connectionInfo, QJsonObject &jsonObject)
{
    qCDebug(connection_manager) << "Filling in connection information from JSON";

    connectionInfo->setParent(this);

    setConnectionInfoFromJsonObject(connectionInfo, jsonObject);

    m_connectionInfoList.push_back(connectionInfo);
    m_connectionInfoDict.insert(connectionInfo->m_name, connectionInfo);
}

void ClientConnectionManager::setActiveConnection(int index)
{
    qCDebug(connection_manager) << "Changing the active connection";

    m_activeConnection = m_connectionInfoList[index];
    emit activeConnectionChanged();

    qCInfo(connection_manager) << "Active connection changed to" << index;
}

void ClientConnectionManager::setConnectionInfoFromJsonObject(ConnectionInfo *connectionInfo,
                                                              QJsonObject &jsonObject)
{
    connectionInfo->m_name = jsonObject[constants::kName].toString();
    connectionInfo->m_transport = static_cast<ConnectionInfo::Transport>(
        jsonObject[constants::kTransport].toInt());

    connectionInfo->m_url.setHost(jsonObject[constants::kAddress].toString());
    connectionInfo->m_url.setPort(jsonObject[constants::kPort].toInt());

    if (!jsonObject[constants::kRemoteUserName].isNull()) {
        connectionInfo->m_remoteUserName = jsonObject[constants::kRemoteUserName].toString();
    }
    if (!jsonObject[constants::kBluetoothAddress].isNull()) {
        connectionInfo->m_bluetoothAddress = QBluetoothAddress(
            jsonObject[constants::kBluetoothAddress].toString());
    }
    if (!jsonObject[constants::kBluetoothUUID].isNull()) {
        connectionInfo->m_bluetoothUUID = QBluetoothUuid(
            jsonObject[constants::kBluetoothUUID].toString());
    }
}

void ClientConnectionManager::setJsonObjectFromConnectionInfo(QJsonObject &jsonObject,
                                                              ConnectionInfo *connectionInfo)
{
    jsonObject[constants::kTransport] = static_cast<int>(connectionInfo->m_transport);
    jsonObject[constants::kURL] = connectionInfo->m_url.host();
    jsonObject[constants::kPort] = connectionInfo->m_url.port();
    jsonObject[constants::kBluetoothAddress] = connectionInfo->m_bluetoothAddress.toString();
    jsonObject[constants::kBluetoothUUID] = connectionInfo->m_bluetoothUUID.toString();
    jsonObject[constants::kRemoteUserName] = connectionInfo->m_remoteUserName;
}
