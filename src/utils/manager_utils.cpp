#include "manager_utils.hpp"
#include <QFile>

bool rewriteConfigFile(const QString &filePath, const QJsonObject &jsonDownloadInfo)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical().nospace() << "Error opening " << filePath << ": " << file.errorString();
        return false;
    }

    file.resize(0);
    file.write(QJsonDocument(jsonDownloadInfo).toJson());

    qInfo().nospace() << "Successful rewriting " << filePath;
    return true;
}
