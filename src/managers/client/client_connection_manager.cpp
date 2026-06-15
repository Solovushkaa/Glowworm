#include "client_connection_manager.hpp"
#include "constants.hpp"
#include "manager_utils.hpp"

Q_LOGGING_CATEGORY(connection_manager, "connection.manager")

ClientConnectionManager::ClientConnectionManager(const QString &savePath, QObject *parent)
    : QAbstractListModel(parent)
    , m_savePath(savePath)
{
    qCDebug(connection_manager) << "ClientConnectionManager - created";
}

ClientConnectionManager::~ClientConnectionManager()
{
    qCDebug(connection_manager) << "ClientConnectionManager - destroyed";
}

int ClientConnectionManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        qCDebug(connection_manager) << "QModelIndex parent is not valid";
        return 0;
    }
    return m_connectionInfoList.size();
}

QVariant ClientConnectionManager::data(const QModelIndex &index, int role) const
{
    qCDebug(connection_manager) << "Getting ClientConnectionManager data for the QML List Model";

    if (!index.isValid() || index.row() >= m_connectionInfoList.size())
        return QVariant();

    ConnectionInfo *info = m_connectionInfoList[index.row()];
    switch (role) {
    case NameRole:
        return info->m_name;
    case ConnectionTypeRole:
        return static_cast<int>(info->m_connectionType);
    case AddressRole:
        return info->m_address;
    case RemoteUserRole:
        return info->m_remoteUserUuid;
    case ConnectionStateRole:
        return static_cast<int>(info->m_connectionState);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ClientConnectionManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = constants::kName.toUtf8();
    roles[ConnectionTypeRole] = constants::kConnectionType.toUtf8();
    roles[AddressRole] = constants::kAddress.toUtf8();
    roles[RemoteUserRole] = constants::kRemoteUserUuid.toUtf8();
    roles[ConnectionStateRole] = constants::kConnectionState.toUtf8();
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
                                            ConnectionInfo::ConnectionType connectionType,
                                            const QString &address,
                                            const QString &remoteUserName,
                                            qint16 defaultMessengerPort,
                                            qint16 defaultTransportPort,
                                            bool isSecureConnection)
{
    qCDebug(connection_manager) << "Adding a new connection";

    if (m_connectionInfoDict.contains(name)) {
        qCInfo(connection_manager) << "A connection with the name" << name << "already exists";
        return false;
    }

    auto connectionInfo = new ConnectionInfo(name,
                                             "",
                                             connectionType,
                                             address,
                                             remoteUserName,
                                             ConnectionInfo::ConnectionState::Disconnected,
                                             defaultMessengerPort,
                                             defaultTransportPort,
                                             isSecureConnection,
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
        emit activeConnectionChanged();
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
    if (property == constants::kConnectionType.toUtf8())
        roles << ConnectionTypeRole;
    if (property == constants::kAddress.toUtf8())
        roles << AddressRole;
    if (property == constants::kRemoteUserUuid.toUtf8())
        roles << RemoteUserRole;
    if (property == constants::kConnectionState.toUtf8())
        roles << ConnectionStateRole;

    static const QVector<int> allRoles{NameRole,
                                       ConnectionTypeRole,
                                       AddressRole,
                                       RemoteUserRole,
                                       ConnectionStateRole};

    QModelIndex idx = this->index(index);

    emit dataChanged(idx, idx, roles.isEmpty() ? allRoles : roles);

    return rewriteSelectAppData(info);
}

bool ClientConnectionManager::readSavedConnections()
{
    return readAppData(*this, m_savePath, m_jsonSavedConnections);
}

bool ClientConnectionManager::rewriteSelectAppData(ConnectionInfo *connectionInfo)
{
    QJsonObject jsonObject;
    setJsonObjectFromConnectionInfo(jsonObject, connectionInfo);
    m_jsonSavedConnections[connectionInfo->m_name] = jsonObject;

    return rewriteAppDataFile(m_savePath, m_jsonSavedConnections);
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
    connectionInfo->m_connectionType = static_cast<ConnectionInfo::ConnectionType>(
        jsonObject[constants::kConnectionType].toInt());
    connectionInfo->m_address = jsonObject[constants::kAddress].toString();
    connectionInfo->m_remoteUserUuid = jsonObject[constants::kRemoteUserUuid].toString();
    connectionInfo->m_connectionState = ConnectionInfo::ConnectionState::Disconnected;
    connectionInfo->m_messengerPort = jsonObject[constants::kMessengerPort].toInt();
    connectionInfo->m_transportPort = jsonObject[constants::kTransportPort].toInt();
    connectionInfo->m_isSecureConnection = jsonObject[constants::kIsSecureConnection].toBool();
}

void ClientConnectionManager::setJsonObjectFromConnectionInfo(QJsonObject &jsonObject,
                                                              ConnectionInfo *connectionInfo)
{
    jsonObject[constants::kName] = connectionInfo->m_name;
    jsonObject[constants::kHostkey] = connectionInfo->m_hostKey;
    jsonObject[constants::kConnectionType] = static_cast<int>(connectionInfo->m_connectionType);
    jsonObject[constants::kAddress] = connectionInfo->m_address;
    jsonObject[constants::kRemoteUserUuid] = connectionInfo->m_remoteUserUuid;
    jsonObject[constants::kConnectionState] = static_cast<int>(connectionInfo->m_connectionState);
    jsonObject[constants::kIsSecureConnection] = connectionInfo->m_isSecureConnection;
    jsonObject[constants::kMessengerPort] = connectionInfo->m_messengerPort;
    jsonObject[constants::kTransportPort] = connectionInfo->m_transportPort;
}
