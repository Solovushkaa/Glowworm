#include "initialize.h"
#include <QDir>
#include <QStandardPaths>
#include "log.h"

Initialize::Initialize()
{
    loadPath("/AppData/Client/", "Client");
    loadPath("/AppData/Server/", "Server");

    // Install custom message handler
    qInstallMessageHandler(messageHandler);
}

void Initialize::loadPath(const char *path, const char *directoryName)
{
    QFileInfo clientDirPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                            + path);
    QDir dir(clientDirPath.absoluteDir());

    qDebug() << dir.absolutePath();
    if (!dir.exists()) {
        dir.mkpath(".");
        qDebug() << directoryName << "directory has been created!";
    }
}
