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

inline constexpr QStringView fakeFilePayload
    = u"C++ gives you the power to build anything — from blazing-fast game engines to the "
      "software that runs spacecraft.No other language balances raw hardware control and "
      "high-level abstraction so perfectly — C++ is simply unmatched.";

} // namespace test

#endif // CLIENTDATA_HPP
