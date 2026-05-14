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
