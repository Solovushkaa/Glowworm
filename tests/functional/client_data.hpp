#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include <QStringView>

namespace test {

inline constexpr QStringView testClientConnectionsFileData = uR"json(
{
    "Connection": {
        "name": "Connection",
        "address": "127.0.0.1",
        "port": 6115,
        "transport": 1,
        "remoteUserName": "",
        "bluetoothAddress": "00:00:00:00:00:00",
        "bluetoothUUID": "00000000-0000-0000-0000-000000000000",
        "connectionState": 0
    }
}
)json";
}

#endif // CLIENTDATA_HPP
