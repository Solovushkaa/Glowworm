#include "downloadinfo.h"
#include <gtest/gtest.h>
#include <QSignalSpy>

using namespace testing;

TEST(DownloadInfo, StateChanging)
{
    DownloadInfo downloadInfo;
    downloadInfo.m_downloadStatus = State::Pause;

    ASSERT_EQ(downloadInfo.m_downloadStatus, State::Pause);
}

TEST(DownloadInfo, SignalStateChanging)
{
    DownloadInfo downloadInfo;

    QSignalSpy spy(&downloadInfo, &DownloadInfo::downloadStatusChanged);

    downloadInfo.setDownloadStatus(State::Active);

    EXPECT_EQ(spy.count(), 1);
}

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
