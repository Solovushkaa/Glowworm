#include <QTemporaryFile>
#include "client_connection_manager.hpp"
#include "saved_connection_data.hpp"
#include <gtest/gtest.h>
#include <memory>

struct ClientConnectionManagerTest : ::testing::Test
{
    ClientConnectionManagerTest() { m_configFile.open(); }
    void SetUp() override
    {
        m_configFile.resize(0);
        m_configFile.flush();
    }
    void TearDown() override { m_clientConnectionManager.reset(); }

    void openEmptyConfig() { openConfig(); }
    void openConfigWithOneElement()
    {
        m_configFile.write(test::oneConnectionData.toUtf8());
        m_configFile.flush();
        openConfig();
    }
    void openConfigWithThreeElements()
    {
        m_configFile.write(test::threeConnectionData.toUtf8());
        m_configFile.flush();
        openConfig();
    }
    void openConfig()
    {
        m_clientConnectionManager = std::make_unique<ClientConnectionManager>(
            m_configFile.fileName());
    }

    std::unique_ptr<ClientConnectionManager> m_clientConnectionManager;
    QTemporaryFile m_configFile;
};

TEST_F(ClientConnectionManagerTest, OpenEmptyConfigFile)
{
    openEmptyConfig();
    ASSERT_TRUE(m_clientConnectionManager->readSavedConnections());
}

TEST_F(ClientConnectionManagerTest, OpenConnectionFileWithOneElement)
{
    openConfigWithOneElement();

    ASSERT_TRUE(m_clientConnectionManager->readSavedConnections());

    ASSERT_EQ(m_clientConnectionManager->getConnections().size(), 1);
}

TEST_F(ClientConnectionManagerTest, OpenConnectionFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_clientConnectionManager->readSavedConnections());

    ASSERT_EQ(m_clientConnectionManager->getConnections().size(), 3);
}

TEST_F(ClientConnectionManagerTest, ReadConnectionFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_clientConnectionManager->readSavedConnections());

    ASSERT_EQ(m_clientConnectionManager->getConnections().size(), 3);

    auto connectionsList = m_clientConnectionManager->getConnections();
    for (auto &el : connectionsList) {
        qDebug() << el.m_name;
        qDebug() << el.m_url.host();
        qDebug() << el.m_url.port();
        qDebug() << static_cast<int>(el.m_transport);
        qDebug() << el.m_remoteUserName;
        qDebug() << el.m_bluetoothAddress;
        qDebug() << el.m_bluetoothUUID;
    }

    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(connectionsList[i].m_url.host(), test::address[i]);
        EXPECT_EQ(connectionsList[i].m_url.port(), test::port[i]);
        EXPECT_EQ(static_cast<int>(connectionsList[i].m_transport), test::transport[i]);
        EXPECT_EQ(connectionsList[i].m_remoteUserName, test::remoteUserName[i]);
        EXPECT_EQ(connectionsList[i].m_bluetoothAddress.toString(), test::bluetoothAddress[i]);
        EXPECT_EQ(connectionsList[i].m_bluetoothUUID.toString(), test::bluetoothUUID[i]);

        ++i;
    }
}

TEST_F(ClientConnectionManagerTest, DeleteConnectionInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_clientConnectionManager->readSavedConnections());

    m_clientConnectionManager->deleteConnection(0, 2);
    ASSERT_EQ(m_clientConnectionManager->getConnections().size(), 2);
}

// TEST_F(ClientConnectionManagerTest, AddDownloadInfoToEmptyConfig)
// {
//     openEmptyConfig();

//     ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

//     DownloadInfo downloadInfo;
//     downloadInfo.m_downloadID = test::downloadID.back().toString();
//     downloadInfo.m_url = test::url.back().toString();
//     downloadInfo.m_downloadState = static_cast<DownloadInfo::DownloadState>(test::downloadStatus.back());
//     downloadInfo.m_hostKey = test::hostKey.back().toString();
//     downloadInfo.m_created = test::created.back().toString();
//     downloadInfo.m_accessed = test::accessed.back().toString();
//     downloadInfo.m_modified = test::modified.back().toString();
//     downloadInfo.m_lastReceivedByte = test::lastReceivedByte.back();
//     downloadInfo.m_name = test::name.back().toString();
//     downloadInfo.m_path = test::path.back().toString();
//     downloadInfo.m_saveName = test::saveName.back().toString();
//     downloadInfo.m_savePath = test::savePath.back().toString();
//     downloadInfo.m_size = test::size.back();

//     m_downloadManager->addDownload(downloadInfo);
//     ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 1);
// }

// TEST_F(ClientConnectionManagerTest, AddDuplicateDownloadInfo)
// {
//     openConfigWithThreeElements();

//     ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

//     auto &dict = m_downloadManager->getDownloadInfoDict();
//     m_downloadManager->addDownload(dict[test::downloadID.back()]);
//     ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);
// }

// TEST_F(ClientConnectionManagerTest, DeleteAddDownloadInfo)
// {
//     openConfigWithThreeElements();

//     ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

//     auto &dict = m_downloadManager->getDownloadInfoDict();
//     auto saveDownloadInfo = dict[test::downloadID.back()];
//     m_downloadManager->deleteDownload(dict[test::downloadID.back()]);
//     ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 2);

//     m_downloadManager->addDownload(saveDownloadInfo);
//     ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);

//     for (int i = 0; i < 3; ++i) {
//         EXPECT_EQ(dict[test::downloadID[i]].m_downloadID, test::downloadID[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_url, test::url[i].toString());
//         EXPECT_EQ(static_cast<int>(dict[test::downloadID[i]].m_downloadState),
//                   test::downloadStatus[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_hostKey, test::hostKey[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_created, test::created[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_accessed, test::accessed[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_modified, test::modified[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_lastReceivedByte, test::lastReceivedByte[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_name, test::name[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_path, test::path[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_saveName, test::saveName[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_savePath, test::savePath[i]);
//         EXPECT_EQ(dict[test::downloadID[i]].m_size, test::size[i]);

//         ++i;
//     }
// }
