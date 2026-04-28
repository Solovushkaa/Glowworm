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

TEST_F(ClientConnectionManagerTest, OpenEmptyConfigFile) {}
