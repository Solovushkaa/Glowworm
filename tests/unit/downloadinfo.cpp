#include "downloadinfo.hpp"
#include <QCoreApplication>
#include <QSignalSpy>
#include <gtest/gtest.h>
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


TEST(DownloadInfo, MemberAccess)
{
    DownloadInfo downloadInfo;
    downloadInfo.m_downloadStatus = State::Pause;

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

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    auto en = std::unique_ptr<QtEnvironment>();
    AddGlobalTestEnvironment(en.get());
    return RUN_ALL_TESTS();
}
