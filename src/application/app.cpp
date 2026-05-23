#include "app.hpp"
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <qqml.h>

App::App(QObject *parent)
    : QObject(parent)
{
    registrateQmlTypes();
}

void App::setGraphicPreferences()
{
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
}

void App::registrateQmlTypes()
{
    qmlRegisterSingletonInstance("GlowwormModule", 1, 0, "Client", &m_client);
    qmlRegisterSingletonInstance("GlowwormModule", 1, 0, "Server", &m_server);
    qmlRegisterSingletonInstance("GlowwormModule",
                                 1,
                                 0,
                                 "ClientConnectionManager",
                                 &m_client.getClientConnectionManager());
    qmlRegisterSingletonInstance("GlowwormModule",
                                 1,
                                 0,
                                 "DownloadManager",
                                 &m_client.getDownloadManager());
    qmlRegisterSingletonInstance("GlowwormModule",
                                 1,
                                 0,
                                 "DirectoryManager",
                                 &m_client.getDirectoryManager());
}
