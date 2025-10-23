#ifndef GUIHTTPCLIENTCOONNECTOR_H
#define GUIHTTPCLIENTCOONNECTOR_H

#include <QObject>

class GUIHttpClientCoonnector : public QObject
{
    Q_OBJECT
public:
    explicit GUIHttpClientCoonnector(QObject *parent = nullptr);

signals:
};

#endif // GUIHTTPCLIENTCOONNECTOR_H
