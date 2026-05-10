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

        qWarning() << "AAAAAAAAAA" << Qt::flush;

        client = std::make_unique<Client>(connectionsFile.fileName(), downloadsFile.fileName());
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
    client->changeConnection(0);
    QSignalSpy spy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(spy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(spy.wait(500)); // 0.5 sec
}

TEST_F(ClientServerMessegingTest, CorrectnessConnectionToServer)
{
    client->changeConnection(0);
    QSignalSpy spy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(spy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(spy.wait(500)); // 0.5 sec

    ASSERT_EQ(spy.takeFirst().at(0).toInt(), 200);
}
