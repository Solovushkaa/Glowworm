#ifndef CLIENTSAVEDCONNECTIONMANAGER_H
#define CLIENTSAVEDCONNECTIONMANAGER_H

#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include "connectioninfo.h"

class ClientSavedConnectionManager : public QObject
{
    Q_OBJECT

public:
    // Сonstructor for reading SavedConnections.json file
    ClientSavedConnectionManager();

    // Read saved connections from file
    bool readSavedConnections();
    // Get all connections
    QList<ConnectionInfo> getConnections() { return m_savedConnections; }
    // Get active connection
    ConnectionInfo getActive() { return *m_activeConnection; }
    // Set active connection
    void setActive(int indx);

    // Add connection from list
    void add(const QString &name, const QString &protocol, const QString &ip, const QString &port);
    // Remove connection from list
    void remove(qint64 deleteIndex, qint64 activeIndex);

    // Checking for empty saved connections
    bool isEmpty() const { return m_savedConnections.empty(); }

private:
    // Overwriting a file with saved connections
    void overwriteSavedConnectionsToFile();

signals:
    // A signal about uploading a file with saved connections
    void connectionsLoaded();
    // A signal indicating a change the active connection
    void activeConnectionsChanged();

private:
    QJsonObject m_jsonSavedConnection; // List of a save connections in json for working with file
    QList<ConnectionInfo> m_savedConnections; // List of a save connections
    ConnectionInfo *m_activeConnection;       // Pointer to active connection

    // All fields of information about downloads
    const QList<const char *> m_checkList = {"ip", "port", "protocol"};

    // Path to save connections
    const QString m_savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                               + "/AppData/Client/SavedConnections.json";
};

#endif // CLIENTSAVEDCONNECTIONMANAGER_H
