#include "downloadmanager.hpp"
#include <QByteArray>
#include <QTemporaryFile>
#include "unfinisheddownloaddata.hpp"
#include <gtest/gtest.h>

struct DownloadManagerTest : ::testing::Test
{
    DownloadManagerTest() { m_configFile.open(); }
    void SetUp() override
    {
        m_configFile.resize(0);
        m_configFile.flush();
    }
    void TearDown() override { m_downloadManager.reset(); }

    void openEmptyConfig() { openConfig(); }
    void openConfigWithOneElement()
    {
        m_configFile.write(test::oneDownloadData.toUtf8());
        m_configFile.flush();
        openConfig();
    }
    void openConfigWithThreeElements()
    {
        m_configFile.write(test::threeDownloadData.toUtf8());
        m_configFile.flush();
        openConfig();
    }
    void openConfig()
    {
        m_downloadManager = std::make_unique<DownloadManager>(m_configFile.fileName());
    }

    std::unique_ptr<DownloadManager> m_downloadManager;
    QTemporaryFile m_configFile;
};

TEST_F(DownloadManagerTest, OpenEmptyConfigFile)
{
    openEmptyConfig();
    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());
}

TEST_F(DownloadManagerTest, OpenConfigFileWithOneElement)
{
    openConfigWithOneElement();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 1);
}

TEST_F(DownloadManagerTest, OpenConfigFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);
}

TEST_F(DownloadManagerTest, ParseAndReadConfigFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);

    auto &dict = m_downloadManager->getDownloadInfoDict();
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(dict[test::downloadID[i]].m_downloadID, test::downloadID[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_URL, test::URL[i].toString());
        EXPECT_EQ(static_cast<int>(dict[test::downloadID[i]].m_downloadStatus),
                  test::downloadStatus[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_hostKey, test::hostKey[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_created, test::created[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_accessed, test::accessed[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_modified, test::modified[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_lastReceivedByte, test::lastReceivedByte[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_name, test::name[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_path, test::path[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_saveName, test::saveName[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_savePath, test::savePath[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_size, test::size[i]);

        ++i;
    }
}

TEST_F(DownloadManagerTest, DeleteDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    auto &dict = m_downloadManager->getDownloadInfoDict();
    auto &saveDownloadInfo = dict[test::downloadID.back()];
    m_downloadManager->deleteFromUnfinishedDownload(saveDownloadInfo);
    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 2);
}

TEST_F(DownloadManagerTest, AddDownloadInfoToEmptyConfig)
{
    openEmptyConfig();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    DownloadInfo downloadInfo;
    downloadInfo.m_downloadID = test::downloadID.back().toString();
    downloadInfo.m_URL = test::URL.back().toString();
    downloadInfo.m_downloadStatus = static_cast<State>(test::downloadStatus.back());
    downloadInfo.m_hostKey = test::hostKey.back().toString();
    downloadInfo.m_created = test::created.back().toString();
    downloadInfo.m_accessed = test::accessed.back().toString();
    downloadInfo.m_modified = test::modified.back().toString();
    downloadInfo.m_lastReceivedByte = test::lastReceivedByte.back();
    downloadInfo.m_name = test::name.back().toString();
    downloadInfo.m_path = test::path.back().toString();
    downloadInfo.m_saveName = test::saveName.back().toString();
    downloadInfo.m_savePath = test::savePath.back().toString();
    downloadInfo.m_size = test::size.back();

    m_downloadManager->addDownloadToUnfinished(downloadInfo);
    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 1);
}

TEST_F(DownloadManagerTest, AddDuplicateDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    auto &dict = m_downloadManager->getDownloadInfoDict();
    m_downloadManager->addDownloadToUnfinished(dict[test::downloadID.back()]);
    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);
}

TEST_F(DownloadManagerTest, DeleteAddDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());

    auto &dict = m_downloadManager->getDownloadInfoDict();
    auto saveDownloadInfo = dict[test::downloadID.back()];
    m_downloadManager->deleteFromUnfinishedDownload(dict[test::downloadID.back()]);
    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 2);

    m_downloadManager->addDownloadToUnfinished(saveDownloadInfo);
    ASSERT_EQ(m_downloadManager->getDownloadInfoDict().size(), 3);

    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(dict[test::downloadID[i]].m_downloadID, test::downloadID[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_URL, test::URL[i].toString());
        EXPECT_EQ(static_cast<int>(dict[test::downloadID[i]].m_downloadStatus),
                  test::downloadStatus[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_hostKey, test::hostKey[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_created, test::created[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_accessed, test::accessed[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_modified, test::modified[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_lastReceivedByte, test::lastReceivedByte[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_name, test::name[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_path, test::path[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_saveName, test::saveName[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_savePath, test::savePath[i]);
        EXPECT_EQ(dict[test::downloadID[i]].m_size, test::size[i]);

        ++i;
    }
}
