/**
 * @file main.cpp
 * @brief Main function 
 *  Cross-platform network file manager
 * @author Solovey Alexey <solovushka.al@yandex.ru>
 */

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSGRendererInterface>
// #include "app.h"

/**
 * @brief The main function of the program
 * @param argc Number of command line arguments
 * @param argv Сommand line arguments
 * @return Exit code
 */
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    QQmlApplicationEngine engine;

    // App application;

    // qmlRegisterSingletonInstance("DirectClient", 1, 0, "Client", &core.m_guiHttpClientConnector);

    // ClientConnectionManager connectionManager(client);
    // qmlRegisterSingletonInstance("ClientConnectionManager",
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
