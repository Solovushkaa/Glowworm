#include "initialize.hpp"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include "constants.hpp"
#include "log.hpp"

Q_LOGGING_CATEGORY(application_initialize, "app.initialize")

Initialize::Initialize()
{
    setCustomMessageHandler();

    loadPath("/appdata/logs/", "logs");
    loadPath("/appdata/client/", "client");
    loadPath("/appdata/server/", "server");

    initializeSettings();
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

void Initialize::initializeSettings()
{
    using namespace constants;

    QSettings settings;
    if (!settings.contains(kDefaultMessengerPortName)) {
        settings.setValue(kDefaultMessengerPortName, kDefaultMessengerPortValue);
    }
    if (!settings.contains(kSecureMessengerPortName)) {
        settings.setValue(kSecureMessengerPortName, kSecureMessengerPortValue);
    }
    if (!settings.contains(kDefaultTransportPortName)) {
        settings.setValue(kDefaultTransportPortName, kDefaultTransportPortValue);
    }
    if (!settings.contains(kSecureTransportPortName)) {
        settings.setValue(kSecureTransportPortName, kSecureTransportPortValue);
    }
}
