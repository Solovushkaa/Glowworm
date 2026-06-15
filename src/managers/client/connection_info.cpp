#include "connection_info.hpp"
#include "constants.hpp"
#include <stdexcept>

Q_LOGGING_CATEGORY(connection_info, "connection.info")

namespace connections {

QStringView getConnectionInfoMemberName(ConnectionInfoMember member)
{
    using enum ConnectionInfoMember;

    switch (member) {
    case Name:
        return constants::kName;
    case HostKey:
        return constants::kHostkey;
    case ConnectionType:
        return constants::kConnectionType;
    case Address:
        return constants::kAddress;
    case RemoteUserUuid:
        return constants::kRemoteUserUuid;
    case ConnectionState:
        return constants::kConnectionState;
    case MessengerPort:
        return constants::kMessengerPort;
    case TransportPort:
        return constants::kTransportPort;
    case SecureConnection:
        return constants::kIsSecureConnection;
    default:
        throw std::invalid_argument("Unknown ConnectionInfoMember to convert to string");
    }
}

} // namespace connections

ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject(parent)
{
    qCDebug(connection_info) << "ConnectionInfo - created";
}

ConnectionInfo::ConnectionInfo(const QString &name,
                               const QString &hostKey,
                               ConnectionType connectionType,
                               const QString &address,
                               const QString &remoteUserName,
                               ConnectionState connectionState,
                               qint16 messengerPort,
                               qint16 transportPort,
                               bool isSecureConnection,
                               QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_hostKey(hostKey)
    , m_connectionType(connectionType)
    , m_address(address)
    , m_remoteUserUuid(remoteUserName)
    , m_connectionState(connectionState)
    , m_messengerPort(messengerPort)
    , m_transportPort(transportPort)
    , m_isSecureConnection(isSecureConnection)
{
    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- created";
}

ConnectionInfo::ConnectionInfo(const ConnectionInfo &connectionInfo)
{
    *this = connectionInfo;
}

ConnectionInfo &ConnectionInfo::operator=(const ConnectionInfo &connectionInfo)
{
    m_name = connectionInfo.m_name;
    m_hostKey = connectionInfo.m_hostKey;
    m_connectionType = connectionInfo.m_connectionType;
    m_address = connectionInfo.m_address;
    m_remoteUserUuid = connectionInfo.m_remoteUserUuid;
    m_connectionState = connectionInfo.m_connectionState;
    m_messengerPort = connectionInfo.m_messengerPort;
    m_transportPort = connectionInfo.m_transportPort;
    m_isSecureConnection = connectionInfo.m_isSecureConnection;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- copyed";

    return *this;
}

ConnectionInfo::ConnectionInfo(ConnectionInfo &&connectionInfo)
{
    *this = std::move(connectionInfo);
}

ConnectionInfo &ConnectionInfo::operator=(ConnectionInfo &&connectionInfo)
{
    m_name = std::move(connectionInfo.m_name);
    m_hostKey = std::move(connectionInfo.m_hostKey);
    m_connectionType = connectionInfo.m_connectionType;
    m_address = std::move(connectionInfo.m_address);
    m_remoteUserUuid = std::move(connectionInfo.m_remoteUserUuid);
    m_connectionState = connectionInfo.m_connectionState;
    m_connectionState = connectionInfo.m_connectionState;
    m_messengerPort = connectionInfo.m_messengerPort;
    m_transportPort = connectionInfo.m_transportPort;
    m_isSecureConnection = connectionInfo.m_isSecureConnection;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- moved";

    return *this;
}

ConnectionInfo::~ConnectionInfo()
{
    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- destroyed";
}
