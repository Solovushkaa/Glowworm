#include "connection_info.hpp"
#include "constants.hpp"
#include <stdexcept>

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
