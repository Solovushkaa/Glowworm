#include "clientsavedconnectionmanager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

ClientSavedConnectionManager::ClientSavedConnectionManager()
{
    // m_presetPath = QStandardPaths::read();
    read();
}

void ClientSavedConnectionManager::read()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
               + "/AppData/Client/SavedConnections.json");

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open SavedConnections";
        return;
    }

    QJsonParseError parseError;
    m_jsonSavedConnection = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (m_jsonSavedConnection.array().isEmpty()) {
        m_isConnectionPresetsEmpty = true;
        emit connectionsLoaded();
        return;

    } else if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Ошибка парсинга JSON:" << parseError.errorString();
        qDebug() << "Позиция ошибки:" << parseError.offset;
        return;
    }

    QJsonArray jsonArray = m_jsonSavedConnection.array();

    if (jsonArray.size() == 0) {
        m_isConnectionPresetsEmpty = true;
    } else {
        QJsonValue jsonValue;
        QJsonObject jsonObject;
        for(int i = 0; i < jsonArray.size(); ++i) {

            jsonValue = jsonArray[i];
            if(jsonValue.isObject()){
                m_savedConnections.push_back(QVariantHash());
                jsonObject = jsonValue.toObject();

                m_savedConnections[i]["name"] = jsonObject["name"].toString();
                if(jsonObject["protocol"].toString() == "HTTP"){
                    m_savedConnections[i]["url"] = "http://" + jsonObject["ip"].toString() + ":"
                                                   + jsonObject["port"].toString();
                }
            }
        }
        m_isConnectionPresetsEmpty = false;
        m_activeConnection = &m_savedConnections[0];
        qDebug() << "Конфиг прочитан:";
        qDebug() << (*m_activeConnection)["url"].toString();

        // смена url для клиента
        m_client.setUrl((*m_activeConnection)["url"].toUrl());
        emit connectionsLoaded();
    }
}

void ClientSavedConnectionManager::setActive(int indx)
{
    m_activeConnection = &m_savedConnections[indx];
    m_client.setUrl((*m_activeConnection)["url"].toUrl());
}

void ClientSavedConnectionManager::add(const QString &name,
                                           const QString &protocol,
                                           const QString &ip,
                                           const QString &port)
{
    m_savedConnections.push_back(QVariantHash());
    m_savedConnections.back()["name"] = name;
    if(protocol == "HTTP"){
        m_savedConnections.back()["url"] = "http://" + ip + ":" + port;
    }

    QJsonArray jsonArray = m_jsonSavedConnection.array();

    QJsonObject jsonObj;
    jsonObj["name"] = name;
    jsonObj["protocol"] = protocol;
    jsonObj["ip"] = ip;
    jsonObj["port"] = port;

    jsonArray.append(jsonObj);
    m_jsonSavedConnection.setArray(jsonArray);

    rewriteSavedConnectionsToFile();

    m_isConnectionPresetsEmpty = false;

    emit connectionsLoaded();
}

void ClientSavedConnectionManager::remove(const QString &name)
{
    for (auto it = m_savedConnections.begin(); it != m_savedConnections.end(); ++it) {
        if ((*it)["name"] == name) {
            if (name == (*m_activeConnection)["name"]) {
                m_activeConnection = nullptr;
            }
            m_savedConnections.erase(it);
            if (m_activeConnection == nullptr && m_savedConnections.size() != 0) {
                m_activeConnection = &m_savedConnections[0];
            }
        }
    }

    QJsonArray jsonArray = m_jsonSavedConnection.array();
    for (auto it = jsonArray.begin(); it != jsonArray.end(); ++it) {
        if ((*it).toObject()["name"].toString() == name) {
            jsonArray.erase(it);
        }
    }

    m_jsonSavedConnection = QJsonDocument(jsonArray);
    rewriteSavedConnectionsToFile();

    if (m_savedConnections.size() == 0) {
        m_isConnectionPresetsEmpty = true;
    }
}

void ClientSavedConnectionManager::rewriteSavedConnectionsToFile()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
               + "/AppData/Client/SavedConnections.json");

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open presets";
        return;
    }
    file.resize(0);
    file.write(m_jsonSavedConnection.toJson());
}
