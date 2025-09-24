#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include "httpclient.h"
#include "httpserver.h"
#include "initialize.h"
#include "savedconnectionmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    QQmlApplicationEngine engine;

    Initialize initialize;

    HttpServer httpServer;
    httpServer.startHttpServer();

    HttpClient client;
    qmlRegisterSingletonInstance("HttpClient", 1, 0, "Client", &client);

    SavedConnectionManager connectionManager(client);
    qmlRegisterSingletonInstance("SavedConnectionManager",
                                 1,
                                 0,
                                 "ConnectionManager",
                                 &connectionManager);

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
