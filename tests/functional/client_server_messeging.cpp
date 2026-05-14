#include <QSignalSpy>
#include <QTemporaryFile>
#include "client.hpp"
#include "client_data.hpp"
#include "server.hpp"
#include <gtest/gtest.h>

Q_DECLARE_LOGGING_CATEGORY(functional_client_server_messeging)
Q_LOGGING_CATEGORY(functional_client_server_messeging, "functional.cs_messeging")

struct ClientServerMessegingTest : ::testing::Test
{
    ClientServerMessegingTest()
    {
        connectionsFile.open();
        connectionsFile.resize(0);
        connectionsFile.write(test::testClientConnectionsFileData.toUtf8());
        connectionsFile.flush();

        downloadsFile.open();

        client = std::make_unique<Client>(connectionsFile.fileName(), downloadsFile.fileName());
        int connectionIndex = 0;
        client->setActiveConnection(connectionIndex);
        server = std::make_unique<Server>();

        server->startServers();
    }

    void SetUp() override {}
    void TearDown() override {}

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;
    QTemporaryFile connectionsFile;
    QTemporaryFile downloadsFile;
};

TEST_F(ClientServerMessegingTest, ConnectionToServer)
{
    QSignalSpy spy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(spy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(spy.wait(100)); // 0.1 sec
}

TEST_F(ClientServerMessegingTest, CorrectnessConnectionToServer)
{
    QSignalSpy spy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(spy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(spy.wait(100)); // 0.1 sec

    ASSERT_EQ(spy.takeFirst().at(0).toInt(), 200);
}

TEST_F(ClientServerMessegingTest, GetDirectoryFromServer)
{
    QSignalSpy spy(client.get(), &Client::currentDirectoryChanged);
    ASSERT_TRUE(spy.isValid());

    client->getDirectory(QDir::tempPath());
    ASSERT_TRUE(spy.wait(100)); // 0.1 sec

    auto list = spy.takeFirst().at(0).toList();
    qCDebug(functional_client_server_messeging)
        << "Number of files in the directory:" << list.size();
    ASSERT_TRUE(list.size() > 0);
}

TEST_F(ClientServerMessegingTest, CorrectnessGetDirectoryFromServer)
{
    QSignalSpy spy(client.get(), &Client::currentDirectoryChanged);
    ASSERT_TRUE(spy.isValid());

    client->getDirectory(QDir::tempPath());
    ASSERT_TRUE(spy.wait(100)); // 0.1 sec

    auto list = spy.takeFirst().at(0).toList();
    qCDebug(functional_client_server_messeging)
        << "Number of files in the directory:" << list.size();
    ASSERT_TRUE(list.size() > 0);

    bool isFindConnectionsFile = false;
    bool isFindDownloadsFile = false;
    for (auto &fileInfo : list) {
        if (qvariant_cast<FileInfo *>(fileInfo)->m_path == connectionsFile.fileName()) {
            isFindConnectionsFile = true;
            if (isFindDownloadsFile)
                break;
        }
        if (qvariant_cast<FileInfo *>(fileInfo)->m_path == downloadsFile.fileName()) {
            isFindDownloadsFile = true;
            if (isFindConnectionsFile)
                break;
        }
    }
    ASSERT_TRUE(isFindConnectionsFile && isFindDownloadsFile);
}
