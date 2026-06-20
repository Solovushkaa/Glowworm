/**
 * @file client_connection_manager.hpp
 */

#ifndef CLIENTCONNECTIONMANAGER_HPP
#define CLIENTCONNECTIONMANAGER_HPP

#include <QAbstractListModel>
#include <QJsonObject>
#include <QSet>
#include "connection_info.hpp"
#include "constants.hpp"

Q_DECLARE_LOGGING_CATEGORY(connection_manager)

/**
 * @brief The client connection manager class.
 */
class ClientConnectionManager : public QAbstractListModel
{
    Q_OBJECT
    QML_UNCREATABLE("Managed from C++")

    Q_PROPERTY(bool hasActiveConnection READ hasActiveConnection NOTIFY activeConnectionChanged)

    enum ConnectionRoles {
        NameRole = Qt::UserRole + 1,
        ConnectionTypeRole,
        AddressRole,
        RemoteUserRole,
        ConnectionStateRole
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
                                   ConnectionInfo::ConnectionType connectionType,
                                   const QString &address,
                                   const QString &remoteUserName,
                                   qint16 messengerPort = constants::kDefaultMessengerPortValue,
                                   qint16 transportPort = constants::kDefaultTransportPortValue,
                                   bool isSecureConnection = true,
                                   bool temporaryConnection = false);

    Q_INVOKABLE bool addQuickConnection(const QString &connectionKey, bool temporaryConnection);

    Q_INVOKABLE bool addWebDavConnection(const QString &address,
                                         const QString &name,
                                         const QString &webDavUsername,
                                         const QString &webDavPassword,
                                         bool temporaryConnection);
    /**
     * @brief Removing connection
     */
    Q_INVOKABLE bool deleteConnection(int activeIndex, int deleteIndex);
    Q_INVOKABLE bool updateConnection(int index, const QString &property, const QVariant &value);
    Q_INVOKABLE void updateConnection(ConnectionInfo *connectionInfo);

public:
    /**
     * @brief Сonstructor for reading SavedConnections.json file
     */
    ClientConnectionManager(const QString &savePath, QObject *parent = nullptr);
    ~ClientConnectionManager();

    /**
     * @brief Read saved connections from file.
     */
    bool readSavedConnections();

    bool rewriteSelectAppData(ConnectionInfo *connectionInfo);

    void initInfo(ConnectionInfo *connectionInfo, QJsonObject &jsonObject);

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
    int getConnectionCount() const { return m_connectionInfoList.size(); };

    /**
     * @brief Get active connection.
     */
    Q_INVOKABLE ConnectionInfo *getActiveConnection() { return m_activeConnection; }
    /**
     * @brief Set active connection.
     */
    Q_INVOKABLE void setActiveConnection(int index);

    bool hasActiveConnection() const { return m_activeConnection != nullptr; }

signals:
    /**
     * @brief A signal indicating a change the active connection.
     */
    void activeConnectionChanged();

    void connectionAdded();
    void connectionRemoved(int index);

private:
    QList<ConnectionInfo *> m_connectionInfoList;          ///< Owns ConnectionInfo objects
    QHash<ConnectionInfo *, int> m_connectionInfoIndexDict; ///<
    QSet<QString> m_connectionNames;
    QJsonObject m_jsonSavedConnections; ///< List of a save connections in json for working with file

    ConnectionInfo *m_activeConnection{nullptr}; ///< Pointer to active connection

    QString m_savePath; ///< Path to save connections

public:
    using InfoType = ConnectionInfo;
};

#endif // CLIENTCONNECTIONMANAGER_HPP
