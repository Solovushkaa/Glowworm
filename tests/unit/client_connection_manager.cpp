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
        EXPECT_EQ(connectionInfo->m_address, test::savedConnectionsUrl[index]);
        EXPECT_EQ(static_cast<int>(connectionInfo->m_connectionType), test::connectionType[index]);
        EXPECT_EQ(connectionInfo->m_remoteUserUuid, test::remoteUserName[index]);
        EXPECT_EQ(static_cast<int>(connectionInfo->m_connectionState), test::connectionState[index]);
        EXPECT_EQ(connectionInfo->m_isSecureConnection, test::isSecureConnection[index]);
    }

    void addConnectionToManager(int index, bool isDuplicate = false)
    {
        clientConnectionManager->addConnection(test::name[index].toString(),
                                               static_cast<ConnectionInfo::ConnectionType>(
                                                   test::connectionType[index]),
                                               test::savedConnectionsUrl[index].toString(),
                                               test::remoteUserName[index].toString(),
                                               test::isSecureConnection[index]);

        if (!isDuplicate)
            ++expectedConnectionCount;
    }
    void deleteConnectionFromManager(int activeIndex, int deleteIndex)
    {
        clientConnectionManager->deleteConnection(activeIndex, deleteIndex);

        --expectedConnectionCount;
    }

    [[maybe_unused]] void setTestDataToConnectionInfo(ConnectionInfo *connectionInfo, int index)
    {
        connectionInfo->m_name = test::name[index].toString();
        connectionInfo->m_connectionType = static_cast<ConnectionInfo::ConnectionType>(test::connectionType[index]);
        connectionInfo->m_address = test::savedConnectionsUrl[index].toString();
        connectionInfo->m_remoteUserUuid = test::remoteUserName[index].toString();
        connectionInfo->m_connectionState = static_cast<ConnectionInfo::ConnectionState>(
            test::connectionState[index]);
        connectionInfo->m_isSecureConnection = test::isSecureConnection[index];
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
