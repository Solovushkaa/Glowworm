#include "initialize.hpp"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include "constants.hpp"
#include "log.hpp"
#include "ssl_cert.hpp"

Q_LOGGING_CATEGORY(application_initialize, "app.initialize")

Initialize::Initialize()
{
    setCustomMessageHandler();

    loadPath("/appdata/logs/", "logs");
    loadPath("/appdata/client/", "client");
    loadPath("/appdata/server/", "server");

    initializeSettings();

    generateCerts();
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
    if (!settings.contains(kMessengerPortName)) {
        settings.setValue(kMessengerPortName, kDefaultMessengerPortValue);
    }
    if (!settings.contains(kTransportPortName)) {
        settings.setValue(kTransportPortName, kDefaultTransportPortValue);
    }
}

void Initialize::generateCerts()
{
    const QString serverCertPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                   + "/appdata/server/server_cert.crt";
    const QString serverKeyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/appdata/server/server_key.key";

    QFileInfo certFileInfo(serverCertPath);
    QFileInfo keyFileInfo(serverKeyPath);

    if (!(certFileInfo.exists() && certFileInfo.size() > 40)
        && !(keyFileInfo.exists() && keyFileInfo.size() > 40)) {
        if (!generateSelfSignedEcdsaCertificate()) {
            qCFatal(application_initialize) << "Security certificates not created";
        } else {
            qCInfo(application_initialize) << "Security certificates successful created";
        }
    } else {
        qCInfo(application_initialize) << "Security certificates already exists";
    }
}
