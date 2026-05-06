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

    void openEmptySavedConnectionsFile()
    {
        createNewConnectionManager();

        expectedConnectionCount = 0;
    }
    void openSavedConnectionsFileWithOneElement()
    {
        configFile.write(test::oneConnectionData.toUtf8());

        createNewConnectionManager();

        expectedConnectionCount = 1;
    }
    void openSavedConnectionsFileWithThreeElements()
    {
        configFile.write(test::threeConnectionData.toUtf8());

        createNewConnectionManager();

        expectedConnectionCount = 3;
    }
    void createNewConnectionManager()
    {
        configFile.flush();
        clientConnectionManager = std::make_unique<ClientConnectionManager>(configFile.fileName());
    }

    void compareRealAndExpectedValues(ConnectionInfo *connectionInfo, int index)
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

    void addConnectionToManager(int index, bool isDuplicate = false)
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

        if (!isDuplicate)
            ++expectedConnectionCount;
    }
    void deleteConnectionFromManager(int activeIndex, int deleteIndex)
    {
        clientConnectionManager->deleteConnection(0, deleteIndex);

        --expectedConnectionCount;
    }

    [[maybe_unused]] void setTestDataToConnectionInfo(ConnectionInfo *connectionInfo, int index)
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

    int getRealConnectionCount() const { return clientConnectionManager->getConnectionCount(); }

    std::unique_ptr<ClientConnectionManager> clientConnectionManager;
    QTemporaryFile configFile;
    int expectedConnectionCount;
};

TEST_F(ClientConnectionManagerTest, OpenEmptyConnectionsFile)
{
    openEmptySavedConnectionsFile();
    ASSERT_TRUE(clientConnectionManager->readSavedConnections());
}

TEST_F(ClientConnectionManagerTest, OpenConnectionsFileWithOneElement)
{
    openSavedConnectionsFileWithOneElement();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);
}

TEST_F(ClientConnectionManagerTest, OpenConnectionsFileWithThreeElements)
{
    openSavedConnectionsFileWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);
}

TEST_F(ClientConnectionManagerTest, ReadConnectionsFileWithThreeElements)
{
    openSavedConnectionsFileWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);

    auto &connectionInfoDict = clientConnectionManager->getConnectionInfoDict();
    for (int i = 0; i < expectedConnectionCount; ++i) {
        compareRealAndExpectedValues(connectionInfoDict[test::name[i]], i);
    }
}

TEST_F(ClientConnectionManagerTest, DeleteConnectionByIndex)
{
    openSavedConnectionsFileWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    int deleteIndex = 2;
    deleteConnectionFromManager(0, deleteIndex);
    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);
}

TEST_F(ClientConnectionManagerTest, AddConnectionInfoToEmptyConnectionsFile)
{
    openEmptySavedConnectionsFile();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    int addIndex = 0;
    addConnectionToManager(addIndex);
    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);

    compareRealAndExpectedValues(clientConnectionManager->getConnectionInfoList()[addIndex],
                                 addIndex);
}

TEST_F(ClientConnectionManagerTest, AddDuplicateConnectionInfo)
{
    openSavedConnectionsFileWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    int addIndex = 0;
    bool isDuplicate = true;
    addConnectionToManager(addIndex, isDuplicate);
    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);
}

TEST_F(ClientConnectionManagerTest, DeleteAddConenctionInfo)
{
    openSavedConnectionsFileWithThreeElements();

    ASSERT_TRUE(clientConnectionManager->readSavedConnections());

    auto &connectionInfoDict = clientConnectionManager->getConnectionInfoDict();

    int deleteIndex = clientConnectionManager->getConnectionInfoList().indexOf(
        connectionInfoDict[test::name[0]]);
    deleteConnectionFromManager(0, deleteIndex);
    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);

    int addIndex = 0;
    addConnectionToManager(addIndex);
    ASSERT_EQ(getRealConnectionCount(), expectedConnectionCount);

    for (int i = 0; i < expectedConnectionCount; ++i) {
        compareRealAndExpectedValues(connectionInfoDict[test::name[i]], i);
    }
}
