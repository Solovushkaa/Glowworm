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
    case RemoteUserName:
        return constants::kRemoteUserName;
    case ConnectionState:
        return constants::kConnectionState;
    case DefaultMessengerPort:
        return constants::kDefaultMessengerPort;
    case SecureMessengerPort:
        return constants::kSecureMessengerPort;
    case DefaultTransportPort:
        return constants::kDefaultTransportPort;
    case SecureTransportPort:
        return constants::kSecureTransportPort;
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
                               bool isSecureConnection,
                               qint16 defaultMessengerPort,
                               qint16 secureMessengerPort,
                               qint16 defaultTransportPort,
                               qint16 secureTransportPort,
                               QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_hostKey(hostKey)
    , m_connectionType(connectionType)
    , m_address(address)
    , m_remoteUserName(remoteUserName)
    , m_connectionState(connectionState)
    , m_defaultMessengerPort(defaultMessengerPort)
    , m_secureMessengerPort(secureMessengerPort)
    , m_defaultTransportPort(defaultTransportPort)
    , m_secureTransportPort(secureTransportPort)
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
    m_remoteUserName = connectionInfo.m_remoteUserName;
    m_connectionState = connectionInfo.m_connectionState;
    m_defaultMessengerPort = connectionInfo.m_defaultMessengerPort;
    m_secureMessengerPort = connectionInfo.m_secureMessengerPort;
    m_defaultTransportPort = connectionInfo.m_defaultTransportPort;
    m_secureTransportPort = connectionInfo.m_secureTransportPort;
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
    m_remoteUserName = std::move(connectionInfo.m_remoteUserName);
    m_connectionState = connectionInfo.m_connectionState;
    m_connectionState = connectionInfo.m_connectionState;
    m_defaultMessengerPort = connectionInfo.m_defaultMessengerPort;
    m_secureMessengerPort = connectionInfo.m_secureMessengerPort;
    m_defaultTransportPort = connectionInfo.m_defaultTransportPort;
    m_secureTransportPort = connectionInfo.m_secureTransportPort;
    m_isSecureConnection = connectionInfo.m_isSecureConnection;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- moved";

    return *this;
}

ConnectionInfo::~ConnectionInfo()
{
    qCDebug(connection_info) << "ConnectionInfo:" << m_name << "- destroyed";
}
