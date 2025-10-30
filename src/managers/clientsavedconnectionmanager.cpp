#include "clientsavedconnectionmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

ClientSavedConnectionManager::ClientSavedConnectionManager()
{
    if (readSavedConnections()) {
        qDebug() << "Save connections have been successfully read";
    } else {
        qWarning() << "Error reading save connections";
    }
}

bool ClientSavedConnectionManager::readSavedConnections()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Error opening the \"SavedConnections.json\" file";
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parsing error:" << parseError.errorString();
        qWarning() << "Error position:" << parseError.offset;
        return false;
    } else if (jsonDoc.isObject()) {
        if (m_jsonSavedConnection.isEmpty()) {
            emit connectionsLoaded();
            return true;
        }
    } else {
        qWarning() << "JSON is not an object";
        return false;
    }

    m_jsonSavedConnection = jsonDoc.object();

    QList<QString> deleteList;
    for (auto &key : m_jsonSavedConnection.keys()) {
        if (!m_jsonSavedConnection[key].isObject()) {
            qWarning().nospace() << "Saved connection: \"" << key << "\" - has incorrect syntax";
            deleteList.push_back(key);
            continue;
        }

        const QJsonObject &object = m_jsonSavedConnection[key].toObject();
        m_savedConnections.push_back(ConnectionInfo());

        m_savedConnections.back().m_name = object["name"].toString();
        if (object["protocol"].toString() == "HTTP") {
            m_savedConnections.back().m_url = "http://" + object["ip"].toString() + ":"
                                              + object["port"].toString();
        } else if (object["protocol"].toString() == "HTTPS") {
            // ----------------------------------------- //
        }
    }
    m_activeConnection = &m_savedConnections[0];
    qDebug() << "SavedConnections.json have been read:" << (*m_activeConnection).m_url;

    // Delete incorrect connections
    for (auto &key : deleteList) {
        m_jsonSavedConnection.remove(key);
    }

    emit connectionsLoaded();

    return true;
}

void ClientSavedConnectionManager::setActive(int indx)
{
    m_activeConnection = &m_savedConnections[indx];
    emit activeConnectionsChanged();
}

void ClientSavedConnectionManager::add(const QString &name,
                                           const QString &protocol,
                                           const QString &ip,
                                           const QString &port)
{
    m_savedConnections.push_back(ConnectionInfo());
    m_savedConnections.back().m_name = name;
    if(protocol == "HTTP"){
        m_savedConnections.back().m_url = "http://" + ip + ":" + port;
    } else if (protocol == "HTTPS") {
        // -------------------------------- //
    }

    QJsonObject jsonObj;
    jsonObj["name"] = name;
    jsonObj["protocol"] = protocol;
    jsonObj["ip"] = ip;
    jsonObj["port"] = port;

    m_jsonSavedConnection[name] = jsonObj;

    overwriteSavedConnectionsToFile();

    emit connectionsLoaded();
}

// Доделать
void ClientSavedConnectionManager::remove(qint64 deleteIndex, qint64 activeIndex)
{
    if (activeIndex == deleteIndex) {
        m_activeConnection = nullptr;
    }

    m_jsonSavedConnection.remove(m_savedConnections[deleteIndex].m_name);
    m_savedConnections.remove(deleteIndex);

    if (m_activeConnection == nullptr && m_savedConnections.size() != 0) {
        m_activeConnection = &m_savedConnections[0];
    }

    overwriteSavedConnectionsToFile();
}

void ClientSavedConnectionManager::overwriteSavedConnectionsToFile()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open SavedConnections.json";
        return;
    }

    file.resize(0);
    file.write(QJsonDocument(m_jsonSavedConnection).toJson());
}
