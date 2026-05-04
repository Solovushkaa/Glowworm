#include "connection_info.hpp"
#include "constants.hpp"
#include <stdexcept>

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
    default:
        throw std::invalid_argument("Unknown ConnectionInfoMember to convert to string");
    }
}

} // namespace connections

ConnectionInfo::ConnectionInfo(QObject *parent)
    : QObject(parent)
{}

ConnectionInfo::ConnectionInfo(QString name,
                               Transport transport,
                               QUrl url,
                               QString remoteUserName,
                               QBluetoothAddress bluetoothAddress,
                               QBluetoothUuid bluetoothUUID,
                               ConnectionState connectionState)
    : m_name(name)
    , m_transport(transport)
    , m_url(url)
    , m_remoteUserName(remoteUserName)
    , m_bluetoothAddress(bluetoothAddress)
    , m_bluetoothUUID(bluetoothUUID)
    , m_connectionState(connectionState)
{}

ConnectionInfo &ConnectionInfo::operator=(const ConnectionInfo &connectionInfo)
{
    m_name = connectionInfo.m_name;
    m_transport = connectionInfo.m_transport;
    m_url = connectionInfo.m_url;
    m_remoteUserName = connectionInfo.m_remoteUserName;
    m_bluetoothAddress = connectionInfo.m_bluetoothAddress;
    m_bluetoothUUID = connectionInfo.m_bluetoothUUID;
    m_connectionState = connectionInfo.m_connectionState.value();

    return *this;
}

ConnectionInfo &ConnectionInfo::operator=(ConnectionInfo &&connectionInfo)
{
    m_name = std::move(connectionInfo.m_name);
    m_transport = connectionInfo.m_transport;
    m_url = std::move(connectionInfo.m_url);
    m_remoteUserName = std::move(connectionInfo.m_remoteUserName);
    m_bluetoothAddress = std::move(connectionInfo.m_bluetoothAddress);
    m_bluetoothUUID = std::move(connectionInfo.m_bluetoothUUID);
    m_connectionState = connectionInfo.m_connectionState.value();

    return *this;
}
