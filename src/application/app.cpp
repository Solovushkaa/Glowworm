#include "app.h"

App::App(QObject *parent)
    : QObject{parent}
    , m_initializer()
    , m_client()
    , m_server()
{}
