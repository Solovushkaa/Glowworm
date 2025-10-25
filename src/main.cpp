#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include "app.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    QQmlApplicationEngine engine;

    App core;

    // qmlRegisterSingletonInstance("HttpClient", 1, 0, "Client", &core.m_guiHttpClientConnector);

    // ClientSavedConnectionManager connectionManager(client);
    // qmlRegisterSingletonInstance("ClientSavedConnectionManager",
    //                              1,
    //                              0,
    //                              "ConnectionManager",
    //                              &connectionManager);

    // QLoggingCategory::setFilterRules("*.debug=false");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("DFSystem", "Main");

    return app.exec();
}
