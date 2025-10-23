#ifndef APP_H
#define APP_H

#include <QObject>
#include "initialize.h"

class App : public QObject
{
    Q_OBJECT
public:
    explicit App(QObject *parent = nullptr);

private:
    Initialize m_initializer;
};

#endif // APP_H
