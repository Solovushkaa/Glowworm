/**
 * @file main.cpp
 * @brief Main function 
 *  Cross-platform network file manager
 * @author Solovey Alexey <solovushka.al@yandex.ru>
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "app.hpp"

/**
 * @brief The main function of the program
 * @param argc Number of command line arguments
 * @param argv Сommand line arguments
 * @return Exit code
 */
int main(int argc, char *argv[])
{
    QGuiApplication qtApp(argc, argv);

    QQmlApplicationEngine engine;

    App application;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &qtApp,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("GlowwormModule", "Main");

    return qtApp.exec();
}
