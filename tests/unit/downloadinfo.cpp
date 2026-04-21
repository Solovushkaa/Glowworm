#include "downloadinfo.hpp"
#include <QCoreApplication>
#include <QSignalSpy>
#include <gtest/gtest.h>
#include <limits>
#include <memory>

using namespace testing;

class QtEnvironment : public Environment {
public:
    void SetUp() override {
        static int argc;
        static char *argv[0];
        app = std::make_unique<QCoreApplication>(argc, argv);
    }
private:
    std::unique_ptr<QCoreApplication> app;
};

TEST(DownloadInfo, AccessToClassMember)
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
    downloadInfo.m_downloadStatus = State::Pause;

    ASSERT_EQ(downloadInfo.m_hostKey, "host");
    ASSERT_EQ(downloadInfo.m_downloadStatus, State::Pause);
}

TEST(DownloadInfo, SignalStateChanging)
{
    DownloadInfo downloadInfo;

    QSignalSpy spy(&downloadInfo, &DownloadInfo::downloadStatusChanged);
    ASSERT_TRUE(spy.isValid());

    downloadInfo.setDownloadStatus(State::Active);

    EXPECT_EQ(spy.count(), 1);
}

int main(int argc, char *argv[])
{
    InitGoogleTest(&argc, argv);
    auto en = std::unique_ptr<QtEnvironment>();
    AddGlobalTestEnvironment(en.get());
    return RUN_ALL_TESTS();
}
