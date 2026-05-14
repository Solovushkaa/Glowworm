#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include <QStringView>

namespace test {

inline constexpr QStringView testClientConnectionsFileData = uR"json(
{
    "Connection": {
        "name": "Connection",
        "url": "http://127.0.0.1:6115",
        "transport": 1,
        "remoteUserName": "",
        "connectionState": 0,
        "isSecureConnection": true
    }
}
)json";
}

#endif // CLIENTDATA_HPP
