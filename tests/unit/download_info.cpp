#include <QCoreApplication>
#include <QSignalSpy>
#include "download_info.hpp"
#include <gtest/gtest.h>
#include <limits>

TEST(DownloadInfoTest, AccessToClassMember)
{
    DownloadInfo downloadInfo;

    downloadInfo.m_downloadID = "11asa";
    downloadInfo.m_URL = "192.168.0.1:9012";
    downloadInfo.m_hostKey = "host";
    downloadInfo.m_name = "name";
    downloadInfo.m_path = "path";
    downloadInfo.m_saveName = "save name";
    downloadInfo.m_savePath = "save/path";
    downloadInfo.m_size = std::numeric_limits<qint64>::max();
    downloadInfo.m_lastReceivedByte = 5123;
    downloadInfo.m_created = "1992.02.11";
    downloadInfo.m_modified = "1992.02.11";
    downloadInfo.m_accessed = "1992.02.11";
    downloadInfo.m_downloadState = DownloadState::Pause;

    EXPECT_EQ(downloadInfo.m_hostKey, "host");
    ASSERT_EQ(downloadInfo.m_downloadState, DownloadState::Pause);
}

TEST(DownloadInfoTest, SignalStateChanging)
{
    DownloadInfo downloadInfo;

    QSignalSpy spy(&downloadInfo, &DownloadInfo::downloadStateChanged);
    ASSERT_TRUE(spy.isValid());

    downloadInfo.setDownloadState(DownloadState::Active);

    ASSERT_EQ(spy.count(), 1);
}
