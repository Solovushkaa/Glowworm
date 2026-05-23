#include "initialize.hpp"
#include <QDir>
#include <QStandardPaths>
#include "log.hpp"

Q_LOGGING_CATEGORY(application_initialize, "app.initialize")

Initialize::Initialize()
{
    setCustomMessageHandler();

    loadPath("/appdata/logs/", "logs");
    loadPath("/appdata/client/", "client");
    loadPath("/appdata/server/", "server");
}

void Initialize::loadPath(const char *path, const char *directoryName)
{
    QFileInfo clientDirPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                            + path);
    QDir dir(clientDirPath.absoluteDir());

    if (!dir.exists()) {
        qCInfo(application_initialize) << "Creating" << directoryName << "directory";
        if (dir.mkpath(".")) {
            qCInfo(application_initialize)
                << directoryName << "directory created successfully:" << dir.absolutePath();
        } else {
            qCCritical(application_initialize)
                << "error creating" << directoryName << "directory:" << dir.absolutePath();
        }
    }
}
