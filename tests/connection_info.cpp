#include "connection_info.hpp"
#include <QSignalSpy>
#include <gtest/gtest.h>

TEST(ConnectionInfoTest, ClassVariableType)
{
    ConnectionInfo connectionInfo;
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_name), QString>();
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_transport), ConnectionInfo::Transport>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_url), QUrl>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothAddress), QBluetoothAddress>();
    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothUUID), QBluetoothUuid>();

    ::testing::StaticAssertTypeEq<decltype(connectionInfo.m_remoteUserName), QString>();
}

TEST(ConnectionInfoTest, SignalStateChanging)
{
    ConnectionInfo connectionInfo;

    QSignalSpy spy(&connectionInfo, &ConnectionInfo::connectionStateChanged);
    ASSERT_TRUE(spy.isValid());

    connectionInfo.setConnectionState(ConnectionInfo::ConnectionState::Error);

    ASSERT_EQ(spy.count(), 1);
}
