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

    void openEmptyUnfinishedDownloadsFile()
    {
        createNewDownloadManager();

        expectedDownloadCount = 0;
    }
    void openUnfinishedDownloadsFileWithOneElement()
    {
        configFile.write(test::oneDownloadData.toUtf8());

        createNewDownloadManager();

        expectedDownloadCount = 1;
    }
    void openUnfinishedDownloadsFileWithThreeElements()
    {
        configFile.write(test::threeDownloadData.toUtf8());

        createNewDownloadManager();

        expectedDownloadCount = 3;
    }
    void createNewDownloadManager()
    {
        configFile.flush();
        downloadManager = std::make_unique<DownloadManager>(configFile.fileName());
    }

    void compareRealAndExpectedValues(DownloadInfo *downloadInfo, int index)
    {
        EXPECT_EQ(downloadInfo->m_downloadID, test::downloadID[index]);
        EXPECT_EQ(downloadInfo->m_url, test::unfinishedDownloadsUrl[index].toString());
        EXPECT_EQ(static_cast<int>(downloadInfo->m_downloadState), test::downloadState[index]);
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

    void addDownloadToManager(int index, bool isDuplicate = false)
    {
        downloadManager->addDownload(test::downloadID[index].toString(),
                                     test::unfinishedDownloadsUrl[index].toString(),
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

        if (!isDuplicate)
            ++expectedDownloadCount;
    }

    void deleteDownloadFromManager(int deleteIndex)
    {
        downloadManager->deleteDownload(deleteIndex);

        --expectedDownloadCount;
    }
    void deleteDownloadFromManager(DownloadInfo *downloadInfo)
    {
        downloadManager->deleteDownload(downloadInfo);

        --expectedDownloadCount;
    }

    int getRealDownloadCount() const { return downloadManager->getDownloadCount(); }

    std::unique_ptr<DownloadManager> downloadManager;
    QTemporaryFile configFile;
    int expectedDownloadCount;
};

TEST_F(DownloadManagerTest, OpenEmptyDownloadsFile)
{
    openEmptyUnfinishedDownloadsFile();
    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());
}

TEST_F(DownloadManagerTest, OpenDownloadsFileWithOneElement)
{
    openUnfinishedDownloadsFileWithOneElement();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);
}

TEST_F(DownloadManagerTest, OpenDownloadsFileWithThreeElements)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);
}

TEST_F(DownloadManagerTest, ReadDownloadsFileWithThreeElements)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();
    for (int i = 0; i < 3; ++i) {
        compareRealAndExpectedValues(downloadInfoDict[test::downloadID[i]], i);
    }
}

TEST_F(DownloadManagerTest, DeleteDownloadByIndex)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    int deleteIndex = 0;
    deleteDownloadFromManager(deleteIndex);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);
}

TEST_F(DownloadManagerTest, DeleteDownloadByValue)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    deleteDownloadFromManager(downloadManager->getDownloadInfoDict()[test::downloadID.back()]);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);
}

TEST_F(DownloadManagerTest, AddDownloadInfoToEmptyDownloadsFile)
{
    openEmptyUnfinishedDownloadsFile();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    int addIndex = 0;
    addDownloadToManager(addIndex);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);

    compareRealAndExpectedValues(downloadManager->getDownloadInfoList()[addIndex], addIndex);
}

TEST_F(DownloadManagerTest, AddDuplicateDownloadInfo)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    int addIndex = 0;
    bool isDuplicate = true;
    addDownloadToManager(addIndex, isDuplicate);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);
}

TEST_F(DownloadManagerTest, DeleteAddDownloadInfo)
{
    openUnfinishedDownloadsFileWithThreeElements();

    ASSERT_TRUE(downloadManager->readUnfinishedDownloads());

    auto &downloadInfoDict = downloadManager->getDownloadInfoDict();

    auto deleteElement = downloadInfoDict[test::downloadID.back()];
    deleteDownloadFromManager(deleteElement);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);

    int addIndex = 2;
    addDownloadToManager(addIndex);
    ASSERT_EQ(getRealDownloadCount(), expectedDownloadCount);

    for (int i = 0; i < expectedDownloadCount; ++i) {
        compareRealAndExpectedValues(downloadInfoDict[test::downloadID[i]], i);
    }
}
