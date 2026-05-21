#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include <QStringView>

namespace test {

inline constexpr QStringView testClientConnectionsFileData = uR"json(
{
    "Connection": {
        "name": "Connection",
        "hostKey": "hjk123h12j3k1h2j6kb1h",
        "url": "http://127.0.0.1:6115",
        "connectionType": 0,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    }
}
)json";
}

#endif // CLIENTDATA_HPP
