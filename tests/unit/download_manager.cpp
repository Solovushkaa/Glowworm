#include "download_manager.hpp"
#include <QByteArray>
#include <QTemporaryFile>
#include "unfinished_download_data.hpp"
#include <gtest/gtest.h>

struct DownloadManagerTest : ::testing::Test
{
    DownloadManagerTest() { configFile.open(); }
    void SetUp() override
    {
        configFile.resize(0);
        configFile.flush();
    }
    void TearDown() override { downloadManager.reset(); }

    void openEmptyConfig() { openConfig(); }
    void openConfigWithOneElement()
    {
        configFile.write(test::oneDownloadData.toUtf8());
        openConfig();
    }
    void openConfigWithThreeElements()
    {
        configFile.write(test::threeDownloadData.toUtf8());
        openConfig();
    }
    void openConfig()
    {
        configFile.flush();
        downloadManager = std::make_unique<DownloadManager>(configFile.fileName());
    }

    void compareRealAndEstimateValues(DownloadInfo *downloadInfo, int index)
    {
        EXPECT_EQ(downloadInfo->m_downloadID, test::downloadID[index]);
        EXPECT_EQ(downloadInfo->m_url, test::url[index].toString());
        EXPECT_EQ(static_cast<int>(downloadInfo->downloadState()), test::downloadState[index]);
        EXPECT_EQ(downloadInfo->m_hostKey, test::hostKey[index]);
        EXPECT_EQ(downloadInfo->m_created, test::created[index]);
        EXPECT_EQ(downloadInfo->m_accessed, test::accessed[index]);
        EXPECT_EQ(downloadInfo->m_modified, test::modified[index]);
        EXPECT_EQ(downloadInfo->m_lastReceivedByte, test::lastReceivedByte[index]);
        EXPECT_EQ(downloadInfo->m_name, test::fileName[index]);
        EXPECT_EQ(downloadInfo->m_path, test::path[index]);
        EXPECT_EQ(downloadInfo->m_saveName, test::saveName[index]);
        EXPECT_EQ(downloadInfo->m_savePath, test::savePath[index]);
        EXPECT_EQ(downloadInfo->m_size, test::size[index]);
    }

    void addDownloadToManager(int index)
    {
        downloadManager->addDownload(test::downloadID[index].toString(),
                                     test::url[index].toString(),
                                     test::hostKey[index].toString(),
                                     test::fileName[index].toString(),
                                     test::path[index].toString(),
                                     test::saveName[index].toString(),
                                     test::savePath[index].toString(),
                                     test::size[index],
                                     test::lastReceivedByte[index],
                                     test::created[index].toString(),
                                     test::modified[index].toString(),
                                     test::accessed[index].toString(),
                                     static_cast<DownloadInfo::DownloadState>(
                                         test::downloadState[index]));
    }

    std::unique_ptr<DownloadManager> downloadManager;
    QTemporaryFile configFile;
};

TEST_F(DownloadManagerTest, OpenEmptyDownloadsFile)
{
    openEmptyConfig();
    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());
}

TEST_F(DownloadManagerTest, OpenDownloadsFileWithOneElement)
{
    openConfigWithOneElement();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(downloadManager->getDownloadInfoDict().size(), 1);
}

TEST_F(DownloadManagerTest, OpenDownloadsFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(downloadManager->getDownloadInfoDict().size(), 3);
}

TEST_F(DownloadManagerTest, ReadDownloadsFileWithThreeElements)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(downloadManager->getDownloadInfoDict().size(), 3);

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();
    for (int i = 0; i < 3; ++i) {
        compareRealAndEstimateValues(downloadInfoDict[test::downloadID[i]], i);
    }
}

TEST_F(DownloadManagerTest, DeleteDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();

    auto &saveDownloadInfo = downloadInfoDict[test::downloadID.back()];

    downloadManager->removeDownload(saveDownloadInfo);

    ASSERT_EQ(downloadManager->getDownloadInfoDict().size(), 2);
}

TEST_F(DownloadManagerTest, AddDownloadInfoToEmptyDownloadsFile)
{
    openEmptyConfig();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    addDownloadToManager(0);
    ASSERT_EQ(downloadManager->getDownloadInfoList().size(), 1);
}

TEST_F(DownloadManagerTest, AddDuplicateDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();

    addDownloadToManager(0);
    ASSERT_EQ(downloadManager->getDownloadInfoList().size(), 3);
}

TEST_F(DownloadManagerTest, DeleteAddDownloadInfo)
{
    openConfigWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();

    auto saveDownloadInfo = new DownloadInfo(*downloadInfoDict[test::downloadID.back()]);

    downloadManager->removeDownload(downloadInfoDict[test::downloadID.back()]);
    ASSERT_EQ(downloadInfoDict.size(), 2);

    addDownloadToManager(2);
    ASSERT_EQ(downloadManager->getDownloadInfoList().size(), 3);

    for (int i = 0; i < 3; ++i) {
        compareRealAndEstimateValues(downloadInfoDict[test::downloadID[i]], i);
    }
}
