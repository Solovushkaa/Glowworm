#include <QDir>
#include <QSignalSpy>
#include <QTemporaryFile>
#include "client.hpp"
#include "client_data.hpp"
#include "server.hpp"
#include <gtest/gtest.h>

Q_DECLARE_LOGGING_CATEGORY(functional_client_server_messeging)
Q_LOGGING_CATEGORY(functional_client_server_messeging, "functional.c_s.messeging")

struct ClientServerMessegingTest : ::testing::Test
{
    ClientServerMessegingTest()
    {
        connectionsFile.open();
        connectionsFile.write(test::testClientConnectionsFileData.toUtf8());
        connectionsFile.flush();

        fakeFile.open();
        fakeFile.write(test::fakeFilePayload.toUtf8());
        fakeFile.flush();

        downloadsFile.open();

        client = std::make_unique<Client>(connectionsFile.fileName(), downloadsFile.fileName());
        int connectionIndex = 0;
        client->setActiveConnection(connectionIndex);
        server = std::make_unique<Server>();

        server->startDefaultServer();
    }

    void SetUp() override {}
    void TearDown() override {}

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    QTemporaryFile connectionsFile;
    QTemporaryFile downloadsFile;

    QTemporaryFile fakeFile;
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

TEST_F(ClientServerMessegingTest, GettingDirectoryFromServer)
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

TEST_F(ClientServerMessegingTest, CorrectnessGettingDirectoryFromServer)
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

TEST_F(ClientServerMessegingTest, GettingFileFromServer)
{
    // A temporary file opened with QTemporaryFile is not visible outside
    // the object until another descriptor to this file is opened with QFile
    QFile file(fakeFile.fileName());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));

    QSignalSpy connectionSpy(client.get(), &Client::connectionStatusCodeChanged);
    ASSERT_TRUE(connectionSpy.isValid());

    client->checkConnectionToServer();
    ASSERT_TRUE(connectionSpy.wait(100)); // 0.1 sec

    QSignalSpy directorySpy(client.get(), &Client::currentDirectoryChanged);
    ASSERT_TRUE(directorySpy.isValid());

    client->getDirectory(QDir::tempPath());
    ASSERT_TRUE(directorySpy.wait(100)); // 0.1 sec

    auto list = directorySpy.takeFirst().at(0).toList();

    bool isFindFakeFile = false;
    int fakeFileIndex = 0;
    qCDebug(functional_client_server_messeging) << fakeFile.fileName();
    for (auto &fileInfo : list) {
        if (qvariant_cast<FileInfo *>(fileInfo)->m_path == fakeFile.fileName()) {
            isFindFakeFile = true;
            break;
        }
        ++fakeFileIndex;
    }
    ASSERT_TRUE(isFindFakeFile);

    fakeFile.close();

    QString fileName = "savedFakeFile.txt";
    QString filePath = QDir::tempPath() + "/" + fileName;

    QSignalSpy fileSpy(client.get(), &Client::fileReceived);
    ASSERT_TRUE(fileSpy.isValid());

    client->getFile(fakeFileIndex, fileName, filePath);
    ASSERT_TRUE(fileSpy.wait(200)); // 0.2 sec

    auto savedFilePath = fileSpy.takeFirst().at(0).toString();
    ASSERT_EQ(filePath, savedFilePath);

    QFile savedFile(filePath);
    ASSERT_TRUE(savedFile.open(QIODevice::ReadOnly));
    ASSERT_EQ(QString(savedFile.readAll()), test::fakeFilePayload);
}
