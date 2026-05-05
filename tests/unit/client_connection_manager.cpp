#include <QTemporaryFile>
#include "client_connection_manager.hpp"
#include "saved_connection_data.hpp"
#include <gtest/gtest.h>
#include <memory>

struct ClientConnectionManagerTest : ::testing::Test
{
    ClientConnectionManagerTest() { configFile.open(); }
    void SetUp() override
    {
        configFile.resize(0);
        configFile.flush();
    }
    void TearDown() override { clientConnectionManager.reset(); }

    void openEmptyConfig() { openConfig(); }
    void openConfigWithOneElement()
    {
        configFile.write(test::oneConnectionData.toUtf8());
        configFile.flush();
        openConfig();
    }
    void openConfigWithThreeElements()
    {
        configFile.write(test::threeConnectionData.toUtf8());
        configFile.flush();
        openConfig();
    }
    void openConfig()
    {
        clientConnectionManager = std::make_unique<ClientConnectionManager>(configFile.fileName());
    }

    void compareRealAndEstimateValues(ConnectionInfo *connectionInfo, int index)
    {
        EXPECT_EQ(connectionInfo->m_url.host(), test::address[index]);
        EXPECT_EQ(connectionInfo->m_url.port(), test::port[index]);
        EXPECT_EQ(static_cast<int>(connectionInfo->m_transport), test::transport[index]);
        EXPECT_EQ(connectionInfo->m_remoteUserName, test::remoteUserName[index]);
        EXPECT_EQ(connectionInfo->m_bluetoothAddress.toString(), test::bluetoothAddress[index]);
        EXPECT_EQ(connectionInfo->m_bluetoothUUID.toString(), test::bluetoothUUID[index]);
        EXPECT_EQ(static_cast<int>(connectionInfo->m_connectionState.value()),
                  test::connectionState[index]);
    }

    void addConnectionToManager(int index)
    {
        QUrl url;
        url.setHost(test::address[index].toString());
        url.setPort(test::port[index]);

        clientConnectionManager
            ->addConnection(test::name[index].toString(),
                            static_cast<ConnectionInfo::Transport>(test::transport[index]),
                            url,
                            test::remoteUserName[index].toString(),
                            QBluetoothAddress(test::bluetoothAddress[index].toString()),
                            QBluetoothUuid(test::bluetoothUUID[index]));
    }

    void setTestDataToConnectionInfo(ConnectionInfo *connectionInfo, int index)
    {
        connectionInfo->m_name = test::name[index].toString();
        connectionInfo->m_transport = static_cast<ConnectionInfo::Transport>(test::transport[index]);
        connectionInfo->m_url.setHost(test::address[index].toString());
        connectionInfo->m_url.setPort(test::port[index]);
        connectionInfo->m_remoteUserName = test::remoteUserName[index].toString();
        connectionInfo->m_bluetoothAddress = QBluetoothAddress(
            test::bluetoothAddress[index].toString());
        connectionInfo->m_bluetoothUUID = QBluetoothUuid(test::bluetoothUUID[index]);
        connectionInfo->m_connectionState = static_cast<ConnectionInfo::ConnectionState>(
            test::connectionState[index]);
    }

    std::unique_ptr<ClientConnectionManager> clientConnectionManager;
    QTemporaryFile configFile;
    const int one = 1;
    const int two = 2;
    const int three = 3;
};

TEST_F(ClientConnectionManagerTest, OpenEmptyConnectionsFile)
{
    openEmptyConfig();
    ASSERT_TRUE(clientConnectionManager->readSavedConnections());
}

TEST_F(ClientConnectionManagerTest, OpenConnectionsFileWithOneElement)
{
    openConfigWithOneElement();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), one);
}

TEST_F(ClientConnectionManagerTest, OpenConnectionsFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), three);
}

TEST_F(ClientConnectionManagerTest, ReadConnectionsFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), three);

    auto &connectionInfoDict = clientConnectionManager->getConnectionInfoDict();

    for (int i = 0; i < three; ++i) {
        compareRealAndEstimateValues(connectionInfoDict[test::name[i]], i);
    }
}

TEST_F(ClientConnectionManagerTest, DeleteConnectionInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    clientConnectionManager->removeConnection(0, two);
    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), two);
}

TEST_F(ClientConnectionManagerTest, AddConnectionInfoToEmptyConnectionsFile)
{
    openEmptyConfig();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    addConnectionToManager(0);
    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), one);

    compareRealAndEstimateValues(clientConnectionManager->getConnectionInfoList()[0], 0);
}

TEST_F(ClientConnectionManagerTest, AddDuplicateConnectionInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    addConnectionToManager(0);
    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), 3);
}

TEST_F(ClientConnectionManagerTest, DeleteAddDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    auto &connectionInfoDict = clientConnectionManager->getConnectionInfoDict();

    int removeIndex = clientConnectionManager->getConnectionInfoList().indexOf(
        connectionInfoDict[test::name[0]]);
    clientConnectionManager->removeConnection(0, removeIndex);
    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), 2);

    addConnectionToManager(0);
    ASSERT_EQ(clientConnectionManager->getConnectionInfoList().size(), 3);

    for (int i = 0; i < three; ++i) {
        compareRealAndEstimateValues(connectionInfoDict[test::name[i]], i);
    }
}
