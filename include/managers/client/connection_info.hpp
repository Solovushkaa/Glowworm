/**
 * @file conncetion_info.hpp
 * @brief Provides support for network connections.
 */

#ifndef CONNECTIONINFO_HPP
#define CONNECTIONINFO_HPP

#include <QLoggingCategory>
#include <QObject>
#include <QProperty>
#include <QStringView>
#include <QUrl>
#include <QUuid>
#include <QtQmlIntegration/qqmlintegration.h>

Q_DECLARE_LOGGING_CATEGORY(connection_info)

namespace connections {

/**
 * @brief All fields of information about connections.
 */
enum class ConnectionInfoMember {
    Name,
    HostKey,
    ConnectionType,
    Address,
    RemoteUserUuid,
    ConnectionState,
    MessengerPort,
    TransportPort,
    SecureConnection,
    TemporaryConnection,
    WebDavUsername,
    WebDavPassword,
    WebDavPort,
    WebDavConnection,
    COUNT // To get the size of the enum
};

/**
 * @brief All fields of information about downloads.
 * @param member Enum
 * @return Enum type converted into QStringView
 */
QStringView getConnectionInfoMemberName(ConnectionInfoMember member);

} // namespace connections

/**
 * @class ConnectionInfo
 * @brief -
 */
class ConnectionInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed from C++")

public:
    enum class ConnectionType { Direct, Relay };
    Q_ENUM(ConnectionType)

    enum class ConnectionState {
        Disconnected, ///< No connection
        Connected,    ///< Connected
        Error         ///< Connection error
    };
    Q_ENUM(ConnectionState)

private:
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(ConnectionType connectionType MEMBER m_connectionType NOTIFY connectionTypeChanged)
    Q_PROPERTY(QString address MEMBER m_address NOTIFY addressChanged)
    Q_PROPERTY(ConnectionState connectionState MEMBER m_connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(bool webDavConnection MEMBER m_webDavConnection)

    // --- Constructors ---
public:
    explicit ConnectionInfo(QObject *parent = nullptr);
    ConnectionInfo(const QString &name,
                   const QString &hostKey,
                   ConnectionType connectionType,
                   const QString &address,
                   const QString &remoteUserUuid,
                   ConnectionState connectionState,
                   qint16 messengerPort,
                   qint16 transportPort,
                   bool isSecureConnection = true,
                   bool temporaryConnection = false,
                   const QString &webDavUsername = "",
                   const QString &webDavPassword = "",
                   qint16 webDavPort = 0,
                   bool webDavConnection = false,
                   QObject *parent = nullptr);
    ConnectionInfo(const QString &address,
                   const QString &name,
                   const QString &webDavUsername,
                   const QString &webDavPassword,
                   qint16 webDavPort = 443,
                   bool temporaryConnection = false,
                   QObject *parent = nullptr);

    ConnectionInfo(const ConnectionInfo &connectionInfo);
    ConnectionInfo &operator=(const ConnectionInfo &connectionInfo);

    ConnectionInfo(ConnectionInfo &&connectionInfo);
    ConnectionInfo &operator=(ConnectionInfo &&connectionInfo);

    ~ConnectionInfo();

    // --- Set Methods ---
public:
    void setConnectionState(ConnectionState state)
    {
        qDebug() << "Connection state change";
        m_connectionState = state;
        emit connectionStateChanged();
    }

    // --- Signals ---
signals:
    void nameChanged();
    void connectionTypeChanged();
    void addressChanged();
    void connectionStateChanged();

public:
    QString m_name;        ///< Connection name
    QString m_hostKey;     ///< Host key

    ConnectionType m_connectionType; ///< Network connectionType

    QString m_address;        ///< Ip-address
    QString m_remoteUserUuid; ///<

    ConnectionState m_connectionState; ///< Connection state

    qint16 m_messengerPort;
    qint16 m_transportPort;

    bool m_isSecureConnection;
    bool m_temporaryConnection;

    QString m_webDavUsername;
    QString m_webDavPassword;
    qint16 m_webDavPort;
    bool m_webDavConnection;
};

#endif // CONNECTIONINFO_HPP
