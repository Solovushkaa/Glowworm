#include "connection_info.hpp"
#include <QSignalSpy>
#include <gtest/gtest.h>

TEST(ConnectionInfoTest, ClassVariableType)
{
    ConnectionInfo connectionInfo;
    using namespace ::testing;
    StaticAssertTypeEq<decltype(connectionInfo.m_name), QString>();
    StaticAssertTypeEq<decltype(connectionInfo.m_connectionType), ConnectionInfo::ConnectionType>();

    StaticAssertTypeEq<decltype(connectionInfo.m_address), QString>();

    StaticAssertTypeEq<decltype(connectionInfo.m_remoteUserName), QString>();

    StaticAssertTypeEq<decltype(connectionInfo.m_isSecureConnection), bool>();
}

TEST(ConnectionInfoTest, SignalStateChanging)
{
    ConnectionInfo connectionInfo;
    connectionInfo.setConnectionState(ConnectionInfo::ConnectionState::Connected);

    QSignalSpy spy(&connectionInfo, &ConnectionInfo::connectionStateChanged);
    ASSERT_TRUE(spy.isValid());

    connectionInfo.setConnectionState(ConnectionInfo::ConnectionState::Error);

    ASSERT_EQ(spy.count(), 1);
}
