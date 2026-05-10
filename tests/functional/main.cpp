#include <QCoreApplication>
#include <gtest/gtest.h>
#include <memory>

class QtEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        static int argc = 1;
        static char progName[] = "functional_tests";
        static char *argv[] = {progName, nullptr};
        app = std::make_unique<QCoreApplication>(argc, argv);
    }
    void TearDown() override { app.reset(); }

private:
    std::unique_ptr<QCoreApplication> app;
};

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new QtEnvironment());
    return RUN_ALL_TESTS();
}
