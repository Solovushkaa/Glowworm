#include "initialize.h"
#include <QDir>
#include <QStandardPaths>

Initialize::Initialize()
{
    loadPath("/AppData/Client/", "Client");
    loadPath("/AppData/Server/", "Server");
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
