#include "clientconnectionmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include "additional.h"

ClientConnectionManager::ClientConnectionManager()
{
    if (readSavedConnections()) {
        qDebug() << "Save connections have been successfully read";
    } else {
        qWarning() << "Error reading save connections";
    }
}

bool ClientConnectionManager::readSavedConnections()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << R"(Error opening the "SavedConnections.json" file)";
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
            emit connectionListUpdated();
            return true;
        }
    } else {
        qWarning() << "JSON is not an object";
        return false;
    }

    m_jsonSavedConnection = jsonDoc.object();

    QList<QString> deleteList;
    for (auto &key : m_jsonSavedConnection.keys()) {
        // --- Validation ---
        // if (!m_jsonSavedConnection[key].isObject()) {
        //     qWarning().nospace() << R"(Saved connection: ")" << key
        //                          << R"(" - has incorrect syntax)";
        //     deleteList.push_back(key);
        //     continue;
        // }

        const QJsonObject &object = m_jsonSavedConnection[key].toObject();
        m_savedConnections.push_back(ConnectionInfo());

        m_savedConnections.back().m_name = object[constants::name].toString();
        if (object[constants::protocol].toString() == constants::DIRECT) {
            m_savedConnections.back().m_url.setHost(object[constants::address].toString());
            m_savedConnections.back().m_url.port(object[constants::port].toInt());
        } else if (object[constants::protocol].toString() == constants::STUN) {
            // ----------------------------------------- //
        } else if (object[constants::protocol].toString() == constants::TURN) {
            // ----------------------------------------- //
        } else if (object[constants::protocol].toString() == constants::BLUETOOTH) {
            // ----------------------------------------- //
        }
    }
    qDebug() << "SavedConnections.json have been read";

    // Delete incorrect connections
    for (auto &key : deleteList) {
        m_jsonSavedConnection.remove(key);
    }

    emit connectionListUpdated();

    return true;
}

void ClientConnectionManager::setActive(int indx)
{
    m_activeConnection = &m_savedConnections[indx];
    emit activeConnectionsChanged();
}

void ClientConnectionManager::addDirect(const QString &name, const QString &address, int port)
{
    ConnectionInfo connInfo;
    m_savedConnections.back().m_name = name;
    m_savedConnections.back().m_url.setHost(address);
    m_savedConnections.back().m_url.port(port);

    QJsonObject jsonObj;
    jsonObj[constants::name] = name;
    jsonObj[constants::transport] = constants::DIRECT;
    jsonObj[constants::address] = address;
    jsonObj[constants::port] = port;

    insertConnection(std::move(connInfo), std::move(jsonObj), name);
}

void ClientConnectionManager::insertConnection(ConnectionInfo &&connInfo,
                                               QJsonObject &&jsonObj,
                                               const QString &name)
{
    m_savedConnections.emplaceBack(std::move(connInfo));

    m_jsonSavedConnection[name] = std::move(jsonObj);
    overwriteSavedConnectionsToFile();

    emit connectionListUpdated();
}

void ClientConnectionManager::remove(qint64 deleteIndex, qint64 activeIndex)
{
    if (activeIndex == deleteIndex) {
        m_activeConnection = nullptr;
    }

    m_jsonSavedConnection.remove(m_savedConnections[deleteIndex].m_name);
    m_savedConnections.remove(deleteIndex);

    overwriteSavedConnectionsToFile();

    emit connectionListUpdated();
}

void ClientConnectionManager::overwriteSavedConnectionsToFile()
{
    QFile file(m_savePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open SavedConnections.json";
        return;
    }

    file.resize(0);
    file.write(QJsonDocument(m_jsonSavedConnection).toJson());
}
