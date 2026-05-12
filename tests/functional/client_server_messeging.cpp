#include <QSignalSpy>
#include <QTemporaryFile>
#include "client.hpp"
#include "client_data.hpp"
#include "server.hpp"
#include <gtest/gtest.h>

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
        client->changeConnection(connectionIndex);
        server = std::make_unique<Server>();

        server->startServer();
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
    ASSERT_TRUE(spy.wait(500)); // 0.5 sec
}

TEST_F(ClientServerMessegingTest, CorrectnessConnectionToServer)
{
    QSignalSpy spy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(spy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(spy.wait(500)); // 0.5 sec

    ASSERT_EQ(spy.takeFirst().at(0).toInt(), 200);
}

TEST_F(ClientServerMessegingTest, GetDirectoryFromServer)
{
    QSignalSpy spy(client.get(), &Client::currentDirectoryChanged);
    ASSERT_TRUE(spy.isValid());

    client->getDirectory("/home/");
    ASSERT_TRUE(spy.wait(500)); // 0.5 sec

    auto list = spy.takeFirst().at(0).toList();
    qCritical() << "Directory list size:" << list.size();
    ASSERT_TRUE(list.size() > 0);
}
