#ifndef SAVEDCONNECTIONDATA_HPP
#define SAVEDCONNECTIONDATA_HPP

#include <QStringView>
#include <array>

namespace test {

inline constexpr QStringView oneConnectionData = uR"json(
{
    "Connection": {
        "name": "Connection",
        "hostKey": "hjk123h12j3k1h2j6kb1h",
        "address": "98.222.12.12",
        "defaultMessengerPort": 6115,
        "secureMessengerPort": 274,
        "defaultTransportPort": 6821,
        "secureTransportPort": 13119,
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    }
}
)json";

inline constexpr QStringView threeConnectionData = uR"json(

{
    "Test Local Connection": {
        "name": "Test Local Connection",
        "hostKey": "66oj1otblbe12jove12h2",
        "address": "98.222.12.12",
        "defaultMessengerPort": 6115,
        "secureMessengerPort": 274,
        "defaultTransportPort": 6821,
        "secureTransportPort": 13119,
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    },
    "Second Test Local Connection": {
        "name": "Second Test Local Connection",
        "hostKey": "9loj1o1blbe12jove12h2",
        "address": "98.222.12.12",
        "defaultMessengerPort": 6115,
        "secureMessengerPort": 274,
        "defaultTransportPort": 6821,
        "secureTransportPort": 13119,
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": false
    },
    "Connection": {
        "name": "Connection",
        "hostKey": "hjk123h12j3k1h2j6kb1h",
        "address": "98.222.12.12",
        "defaultMessengerPort": 6115,
        "secureMessengerPort": 274,
        "defaultTransportPort": 6821,
        "secureTransportPort": 13119,
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    }
}
)json";

inline constexpr std::array<QStringView, 3> name = {u"Test Local Connection",
                                                    u"Second Test Local Connection",
                                                    u"Connection"};
inline constexpr std::array<QStringView, 3> hostKey = {u"66oj1otblbe12jove12h2",
                                                       u"9loj1o1blbe12jove12h2",
                                                       u"hjk123h12j3k1h2j6kb1h"};
inline constexpr std::array<QStringView, 3> savedConnectionsUrl = {u"98.222.12.12",
                                                                   u"98.222.12.12",
                                                                   u"98.222.12.12"};
inline constexpr std::array<qint16, 3> defaultMessengerPort = {6115, 6115, 6115};
inline constexpr std::array<qint16, 3> secureMessengerPort = {274, 274, 274};
inline constexpr std::array<qint16, 3> defaultTransportPort = {6821, 6821, 6821};
inline constexpr std::array<qint16, 3> secureTransportPort = {13119, 13119, 13119};
inline constexpr std::array<int, 3> connectionType = {1, 1, 1};
inline constexpr std::array<QStringView, 3> remoteUserName = {u"", u"", u""};
inline constexpr std::array<int, 3> connectionState = {0, 0, 0};
inline constexpr std::array<bool, 3> isSecureConnection = {true, false, true};

} // namespace test

#endif // SAVEDCONNECTIONDATA_HPP
