#include "downloadmanager.hpp"
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <gtest/gtest.h>

struct DownloadManagerTest : ::testing::Test
{
    DownloadManagerTest()
    {
        m_data =
            R"json(
{
  "m_hostKey_hostKey_6723yfi1": {
    "downloadID": "8xask982",
    "downloadStatus": 0,
    "hostKey": "6e57f36174",
    "url": "192.168.0.1",
    "created": "Fri Mar 28 20:47:50 2025",
    "accessed": "Thu Sep 11 21:46:58 2025",
    "modified": "Fri Mar 28 20:47:50 2025",
    "lastReceivedByte": 0,
    "name": "CV.docx",
    "path": "/home/alexey/CV.docx",
    "saveName": "CV.docx",
    "savePath": "/home/alexey/TestDownloadDir/",
    "size": 11914
  }
}
            )json";
    }
    void SetUp() override
    {
        m_configFile.open();
        m_configFile.resize(0);
    }
    void TearDown() override {}

    void openEmptyConfig() { openConfig(); }
    void openConfigWithOneElement()
    {
        m_configFile.write(m_data);
        m_configFile.flush();
        openConfig();
    }
    void openConfig()
    {
        m_downloadManager = std::make_unique<DownloadManager>(m_configFile.fileName());
    }

    std::unique_ptr<DownloadManager> m_downloadManager;
    QTemporaryFile m_configFile;
    QByteArray m_data;
};

TEST_F(DownloadManagerTest, CreatingClassInstanceWithEmptyFile)
{
    openEmptyConfig();
    ASSERT_TRUE(m_downloadManager->readUnfinishedDownloads());
}

TEST_F(DownloadManagerTest, CreatingClassInstanceWithFiledFile)
{
    openConfigWithOneElement();
    EXPECT_TRUE(m_downloadManager->readUnfinishedDownloads());
    auto &dict = m_downloadManager->getDownloadInfoDict();
    DownloadInfo el;
    for (auto e : dict) {
        el = e;
    }

    qDebug() << el.m_downloadID;
    qDebug() << static_cast<int>(el.m_downloadStatus);
    qDebug() << el.m_hostKey;
    qDebug() << el.m_created;
    qDebug() << el.m_accessed;
    qDebug() << el.m_modified;
    qDebug() << el.m_lastReceivedByte;
    qDebug() << el.m_name;
    qDebug() << el.m_path;
    qDebug() << el.m_saveName;
    qDebug() << el.m_savePath;
    qDebug() << el.m_size;

    ASSERT_EQ(m_downloadManager->getVariantListUnfinishedDownloads().size(), 1);
}
