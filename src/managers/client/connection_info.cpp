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
    case Transport:
        return constants::kTransport;
    case Address:
        return constants::kAddress;
    case Port:
        return constants::kPort;
    case RemoteUserName:
        return constants::kRemoteUserName;
    case BluetoothAddress:
        return constants::kBluetoothAddress;
    case BluetoothUUID:
        return constants::kBluetoothUUID;
    case ConnectionState:
        return constants::kConnectionState;
    default:
        throw std::invalid_argument("Unknown ConnectionInfoMember to convert to string");
    }
}

} // namespace connections

ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject(parent)
{
    qCDebug(connection_info) << "ConnectionInfo empty object successfully created";
}

ConnectionInfo::ConnectionInfo(const QString &name,
                               Transport transport,
                               const QUrl &url,
                               const QString &remoteUserName,
                               const QBluetoothAddress &bluetoothAddress,
                               const QBluetoothUuid &bluetoothUUID,
                               ConnectionState connectionState,
                               QObject *parent)
    : m_name(name)
    , m_transport(transport)
    , m_url(url)
    , m_remoteUserName(remoteUserName)
    , m_bluetoothAddress(bluetoothAddress)
    , m_bluetoothUUID(bluetoothUUID)
    , m_connectionState(connectionState)
    , QObject(parent)
{
    qCDebug(connection_info) << "ConnectionInfo" << m_name << "object successfully created";
}

ConnectionInfo::ConnectionInfo(const ConnectionInfo &connectionInfo)
{
    *this = connectionInfo;
}

ConnectionInfo &ConnectionInfo::operator=(const ConnectionInfo &connectionInfo)
{
    m_name = connectionInfo.m_name;
    m_transport = connectionInfo.m_transport;
    m_url = connectionInfo.m_url;
    m_remoteUserName = connectionInfo.m_remoteUserName;
    m_bluetoothAddress = connectionInfo.m_bluetoothAddress;
    m_bluetoothUUID = connectionInfo.m_bluetoothUUID;
    m_connectionState = connectionInfo.m_connectionState;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo" << m_name << "object successfully copyed";

    return *this;
}

ConnectionInfo::ConnectionInfo(ConnectionInfo &&connectionInfo)
{
    *this = std::move(connectionInfo);
}

ConnectionInfo &ConnectionInfo::operator=(ConnectionInfo &&connectionInfo)
{
    m_name = std::move(connectionInfo.m_name);
    m_transport = connectionInfo.m_transport;
    m_url = std::move(connectionInfo.m_url);
    m_remoteUserName = std::move(connectionInfo.m_remoteUserName);
    m_bluetoothAddress = std::move(connectionInfo.m_bluetoothAddress);
    m_bluetoothUUID = std::move(connectionInfo.m_bluetoothUUID);
    m_connectionState = connectionInfo.m_connectionState;
    this->setParent(connectionInfo.parent());

    qCDebug(connection_info) << "ConnectionInfo" << m_name << "object successfully moved";

    return *this;
}

ConnectionInfo::~ConnectionInfo()
{
    qCDebug(connection_info) << "ConnectionInfo" << m_name << "object successfully destroyed";
}
