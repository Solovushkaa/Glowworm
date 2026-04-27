#include "connectioninfo.hpp"
#include <gtest/gtest.h>

TEST(ConnectionInfoTest, ClassVariableType)
{
    ConnectionInfo connectionInfo;
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_name), QString>();
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_transport), Transport>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_url), QUrl>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothAddress), QBluetoothAddress>();
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothUUID), QBluetoothUuid>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_userName), QString>();
}
