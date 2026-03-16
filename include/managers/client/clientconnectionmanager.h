/**
 * @file clientconnectionmanager.h
 */

#ifndef CLIENTCONNECTIONMANAGER_H
#define CLIENTCONNECTIONMANAGER_H

#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include "connectioninfo.h"

/**
 * @brief The client connection manager class.
 */
class ClientConnectionManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Сonstructor for reading SavedConnections.json file
     */
    ClientConnectionManager();

    /**
     * @brief Read saved connections from file.
     */
    bool readSavedConnections();
    /**
     * @brief Get all connections.
     */
    QList<ConnectionInfo> getConnections() { return m_savedConnections; }
    /**
     * @brief Get active connection.
     */
    ConnectionInfo *getActive() { return m_activeConnection; }
    /**
     * @brief Set active connection.
     */
    void setActive(int indx);

    /**
     * @brief Saved direct connection.
     */
    void addDirect(const QString &name, const QString &address, int port);
    /**
     * @brief Insert connection into m_savedConnections and m_jsonSavedConnection.
     */
    void insertConnection(ConnectionInfo &&connInfo, QJsonObject &&jsonObj, const QString &name);
    /**
     * @brief Remove connection from list .
     */
    void remove(qint64 deleteIndex, qint64 activeIndex);

    /**
     * @brief Checking for empty saved connections.
     */
    bool isEmptyListConnections() const { return m_savedConnections.empty(); }

    /**
     * @brief Checking connections.
     */
    bool isConnected() const { return (m_activeConnection != nullptr); }

private:
    /**
     * @brief Overwriting a file with saved connections.
     */
    void overwriteSavedConnectionsToFile();

signals:
    /**
     * @brief A signal about uploading a file with saved connections.
     */
    void connectionListUpdated();
    /**
     * @brief A signal indicating a change the active connection.
     */
    void activeConnectionsChanged();

private:
    QJsonObject m_jsonSavedConnection; ///< List of a save connections in json for working with file
    QList<ConnectionInfo> m_savedConnections;    ///< List of a save connections
    ConnectionInfo *m_activeConnection{nullptr}; ///< Pointer to active connection

    // Path to save connections
    const QString m_savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                               + "/AppData/Client/SavedConnections.json";
};

#endif // CLIENTCONNECTIONMANAGER_H
