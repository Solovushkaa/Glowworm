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
    case URL:
        return constants::kURL;
    case BluetoothAddress:
        return constants::kBluetoothAddress;
    case BluetoothUUID:
        return constants::kBluetoothUUID;
    default:
        throw std::invalid_argument("Unknown ConnectionInfoMember to convert to string");
    }
}
