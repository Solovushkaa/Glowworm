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
        "url": "http://98.222.12.12:6115",
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
        "url": "http://98.222.12.12:6115",
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    },
    "Second Test Local Connection": {
        "name": "Second Test Local Connection",
        "hostKey": "9loj1o1blbe12jove12h2",
        "url": "http://98.222.12.12:6115",
        "connectionType": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": false
    },
    "Connection": {
        "name": "Connection",
        "hostKey": "hjk123h12j3k1h2j6kb1h",
        "url": "http://98.222.12.12:6115",
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
inline constexpr std::array<QStringView, 3> savedConnectionsUrl = {u"http://98.222.12.12:6115",
                                                                   u"http://98.222.12.12:6115",
                                                                   u"http://98.222.12.12:6115"};
inline constexpr std::array<int, 3> connectionType = {1, 1, 1};
inline constexpr std::array<QStringView, 3> remoteUserName = {u"", u"", u""};
inline constexpr std::array<int, 3> connectionState = {0, 0, 0};
inline constexpr std::array<bool, 3> isSecureConnection = {true, false, true};

} // namespace test

#endif // SAVEDCONNECTIONDATA_HPP
