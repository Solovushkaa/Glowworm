#include <QStringView>

namespace test {

inline constexpr QStringView oneConnectionData = uR"json(
{
    "Connection": {
        "ip": "127.0.0.1",
        "port": "1234",
        "protocol": "HTTP"
    }
}
)json";

inline constexpr QStringView threeConnectionData = uR"json(

{
    "Test Local Connection": {
        "ip": "127.0.0.1",
        "port": "1234",
        "protocol": "HTTP"
    },
    "Second Test Local Connection": {
        "ip": "127.0.0.1",
        "port": "1235",
        "protocol": "HTTP"
    },
    "Connection": {
        "ip": "127.0.0.1",
        "port": "1234",
        "protocol": "HTTP"
    }
}
)json";

} // namespace test
