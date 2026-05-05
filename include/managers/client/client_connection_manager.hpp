/**
 * @file client_connection_manager.hpp
 */

#ifndef CLIENTCONNECTIONMANAGER_HPP
#define CLIENTCONNECTIONMANAGER_HPP

#include <QAbstractListModel>
#include <QJsonObject>
#include <QStandardPaths>
#include "connection_info.hpp"

/**
 * @brief The client connection manager class.
 */
class ClientConnectionManager : public QAbstractListModel
{
    Q_OBJECT
    QML_UNCREATABLE("Managed from C++")

    enum ConnectionRoles {
        NameRole,
        TransportRole,
        URLRole,
        RemoteUserRole,
        BluetoothAddressRole,
        BluetoothUUIDRole,
        StateRole
    };

    // --- ModelView methods ---
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Adding connection
     */
private:
    bool addConnection(ConnectionInfo *info);

public:
    Q_INVOKABLE bool addConnection(const QString &name,
                                   ConnectionInfo::Transport transport,
                                   const QUrl &url,
                                   const QString &remoteUserName,
                                   const QBluetoothAddress &bluetoothAddress,
                                   const QBluetoothUuid &bluetoothUUID);
    /**
     * @brief Removing connection
     */
    Q_INVOKABLE bool removeConnection(int activeIndex, int deleteIndex);
    Q_INVOKABLE bool updateConnection(int index, const QString &property, const QVariant &value);

public:
    /**
     * @brief Сonstructor for reading SavedConnections.json file
     */
    ClientConnectionManager(
        const QString &savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                  + "/AppData/Client/SavedConnections.json",
        QObject *parent = nullptr);

    /**
     * @brief Read saved connections from file.
     */
    bool readSavedConnections();

    void initInfo(ConnectionInfo *connectionInfo, QJsonObject &jsonObject);
    /**
     * @brief Set active connection.
     */
    void setActive(int index);

    void setConnectionInfoFromJsonObject(ConnectionInfo *connectionInfo, QJsonObject &jsonObject);
    void setJsonObjectFromConnectionInfo(QJsonObject &jsonObject, ConnectionInfo *connectionInfo);

    /**
     * @brief Checking for empty saved connections.
     */
    bool isEmptyListConnections() const { return m_connectionInfoList.empty(); }

    /**
     * @brief Get all connections as a List.
     */
    QList<ConnectionInfo *> &getConnectionInfoList() { return m_connectionInfoList; }
    QHash<QString, ConnectionInfo *> &getConnectionInfoDict() { return m_connectionInfoDict; }

    /**
     * @brief Get active connection.
     */
    ConnectionInfo *getActiveConnection() { return m_activeConnection; }

signals:
    /**
     * @brief A signal indicating a change the active connection.
     */
    void activeConnectionsChanged();

    void connectionAdded();
    void connectionRemoved(int index);

private:
    QList<ConnectionInfo *> m_connectionInfoList;          ///< Owns ConnectionInfo objects
    QHash<QString, ConnectionInfo *> m_connectionInfoDict; ///< Information about connections
    QJsonObject m_jsonSavedConnections; ///< List of a save connections in json for working with file

    ConnectionInfo *m_activeConnection; ///< Pointer to active connection

    QString m_savePath; ///< Path to save connections

public:
    using InfoType = ConnectionInfo;
};

#endif // CLIENTCONNECTIONMANAGER_HPP
