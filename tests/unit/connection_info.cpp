#include "connection_info.hpp"
#include <QSignalSpy>
#include <gtest/gtest.h>

TEST(ConnectionInfoTest, ClassVariableType)
{
    ConnectionInfo connectionInfo;
    using namespace ::testing;
    StaticAssertTypeEq<decltype(connectionInfo.m_name), QString>();
    StaticAssertTypeEq<decltype(connectionInfo.m_transport), ConnectionInfo::Transport>();

    StaticAssertTypeEq<decltype(connectionInfo.m_url), QUrl>();

    StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothAddress), QBluetoothAddress>();
    StaticAssertTypeEq<decltype(connectionInfo.m_bluetoothUUID), QBluetoothUuid>();

    StaticAssertTypeEq<decltype(connectionInfo.m_remoteUserName), QString>();
}

TEST(ConnectionInfoTest, SignalStateChanging)
{
    ConnectionInfo connectionInfo;

    QSignalSpy spy(&connectionInfo, &ConnectionInfo::connectionStateChanged);
    ASSERT_TRUE(spy.isValid());

    connectionInfo.setConnectionState(ConnectionInfo::ConnectionState::Error);

    ASSERT_EQ(spy.count(), 1);
}
