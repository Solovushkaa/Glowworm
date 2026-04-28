/**
 * @file client_connection_manager.hpp
 */

#ifndef CLIENTCONNECTIONMANAGER_HPP
#define CLIENTCONNECTIONMANAGER_HPP

#include <QJsonObject>
#include <QObject>
#include <QStandardPaths>
#include "connection_info.hpp"

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
    ClientConnectionManager(
        const QString &savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/AppData/Client/SavedConnections.json");

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

    void setConnectionInfoFromJson(ConnectionInfo &connectionInfo, QJsonObject &jsonObject);
    void setJsonFromSavedConnections(QJsonObject &jsonObject, ConnectionInfo &connectionInfo);

    // /**
    //  * @brief Saved direct connection.
    //  */
    // void addDirect(const QString &name, const QString &address, int port);
    /**
     * @brief Insert connection into m_savedConnections and m_jsonSavedConnection.
     */
    template<typename ConnInfo>
        requires std::same_as<std::remove_reference_t<ConnInfo>, ConnectionInfo>
    bool addConnection(ConnInfo &&connInfo);
    /**
     * @brief Remove connection from list .
     */
    void remove(qint64 deleteIndex, qint64 activeIndex);

    /**
     * @brief Checking for empty saved connections.
     */
    bool isEmptyListConnections() const { return m_savedConnections.empty(); }

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
    ConnectionInfo *m_activeConnection;          ///< Pointer to active connection

    // Path to save connections
    QString m_savePath;
};

#endif // CLIENTCONNECTIONMANAGER_HPP
