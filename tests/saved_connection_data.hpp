#ifndef SAVEDCONNECTIONDATA_HPP
#define SAVEDCONNECTIONDATA_HPP

#include <QStringView>
#include <array>

namespace test {

inline constexpr QStringView oneConnectionData = uR"json(
{
    "Connection": {
        "name": "Connection",
        "address": "98.222.12.12",
        "port": 1234,
        "transport": 1,
        "remoteUserName": "",
        "bluetoothAddress": "00:00:00:00:00:00",
        "bluetoothUUID": "00000000-0000-0000-0000-000000000000"
    }
}
)json";

inline constexpr QStringView threeConnectionData = uR"json(

{
    "Test Local Connection": {
        "name": "Test Local Connection",
        "address": "98.222.12.12",
        "port": 1234,
        "transport": 1,
        "remoteUserName": "",
        "bluetoothAddress": "00:00:00:00:00:00",
        "bluetoothUUID": "00000000-0000-0000-0000-000000000000"
    },
    "Second Test Local Connection": {
        "name": "Second Test Local Connection",
        "address": "98.222.12.12",
        "port": 1234,
        "transport": 1,
        "remoteUserName": "",
        "bluetoothAddress": "00:00:00:00:00:00",
        "bluetoothUUID": "00000000-0000-0000-0000-000000000000"
    },
    "Connection": {
        "name": "Connection",
        "address": "98.222.12.12",
        "port": 1234,
        "transport": 1,
        "remoteUserName": "",
        "bluetoothAddress": "00:00:00:00:00:00",
        "bluetoothUUID": "00000000-0000-0000-0000-000000000000"
    }
}
)json";

inline constexpr std::array<QStringView, 3> name = {u"Test Local Connection",
                                                    u"Second Test Local Connection",
                                                    u"Connection"};
inline constexpr std::array<QStringView, 3> address = {u"98.222.12.12",
                                                       u"98.222.12.12",
                                                       u"98.222.12.12"};
inline constexpr std::array<int, 3> port = {1234, 1234, 1234};
inline constexpr std::array<int, 3> transport = {1, 1, 1};
inline constexpr std::array<QStringView, 3> remoteUserName = {u"", u"", u""};
inline constexpr std::array<QStringView, 3> bluetoothAddress = {u"00:00:00:00:00:00",
                                                                u"00:00:00:00:00:00",
                                                                u"00:00:00:00:00:00"};
inline constexpr std::array<QStringView, 3> bluetoothUUID
    = {u"{00000000-0000-0000-0000-000000000000}",
       u"{00000000-0000-0000-0000-000000000000}",
       u"{00000000-0000-0000-0000-000000000000}"};

} // namespace test

#endif // SAVEDCONNECTIONDATA_HPP
