/**
 * @file conncetion_info.hpp
 * @brief Provides support for network connections.
 */

#ifndef CONNECTIONINFO_HPP
#define CONNECTIONINFO_HPP

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QObject>
#include <QProperty>
#include <QStringView>
#include <QUrl>
#include <QtQmlIntegration/qqmlintegration.h>

namespace connections {

/**
 * @brief All fields of information about connections.
 */
enum class ConnectionInfoMember {
    Name,
    Address,
    Port,
    Transport,
    RemoteUserName,
    BluetoothAddress,
    BluetoothUUID,
    ConnectionState,
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

    Q_PROPERTY(QString name MEMBER m_name READ name NOTIFY nameChanged)
    Q_PROPERTY(Transport transport MEMBER m_transport READ transport NOTIFY transportChanged)
    Q_PROPERTY(QUrl url MEMBER m_url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString remoteUserName MEMBER m_remoteUserName READ remoteUserName NOTIFY
                   remoteUserNameChanged)
    Q_PROPERTY(QBluetoothAddress bluetoothAddress MEMBER m_bluetoothAddress READ bluetoothAddress
                   NOTIFY bluetoothAddressChanged)
    Q_PROPERTY(QBluetoothUuid bluetoothUUID MEMBER m_bluetoothUUID READ bluetoothUUID NOTIFY
                   bluetoothUUIDChanged)

public:
    enum class Transport {
        DIRECT,    ///< Direct connection with HTTP protocol
        NATTRAV,   ///<
        BLUETOOTH, ///< BlueTooth protocol
        TURN       ///< Indirect connection with HTTP protocol
    };
    Q_ENUM(Transport)

    enum class ConnectionState {
        Disconnected, ///< No connection
        Connected,    ///< Connected
        Error         ///< Connection error
    };
    Q_ENUM(ConnectionState)

private:
    Q_PROPERTY(ConnectionState connectionState READ connectionState WRITE setConnectionState NOTIFY
                   connectionStateChanged BINDABLE bindableConnectionState)

    QBindable<ConnectionState> bindableConnectionState() { return &m_connectionState; }

    // --- Constructors ---
public:
    explicit ConnectionInfo(QObject *parent = nullptr);
    ConnectionInfo(const QString &name,
                   Transport transport,
                   const QUrl &url,
                   const QString &remoteUserName,
                   const QBluetoothAddress &bluetoothAddress,
                   const QBluetoothUuid &bluetoothUUID,
                   ConnectionState connectionState,
                   QObject *parent = nullptr);

    ConnectionInfo(const ConnectionInfo &connectionInfo) { *this = connectionInfo; }
    ConnectionInfo &operator=(const ConnectionInfo &connectionInfo);

    ConnectionInfo(ConnectionInfo &&connectionInfo) { *this = std::move(connectionInfo); }
    ConnectionInfo &operator=(ConnectionInfo &&connectionInfo);

    // --- Methods ---
public:
    QString name() const { return m_name; }
    Transport transport() const { return m_transport; }
    QUrl url() const { return m_url; }
    QString remoteUserName() const { return m_remoteUserName; }
    QBluetoothAddress bluetoothAddress() const { return m_bluetoothAddress; }
    QBluetoothUuid bluetoothUUID() const { return m_bluetoothUUID; }
    ConnectionState connectionState() const { return m_connectionState; }

    // --- Slots ---
public slots:
    void setConnectionState(ConnectionState state) { m_connectionState = state; }

    // --- Signals ---
signals:
    void nameChanged();
    void transportChanged();
    void urlChanged();
    void remoteUserNameChanged();
    void bluetoothAddressChanged();
    void bluetoothUUIDChanged();
    void connectionStateChanged();

public:
    // --- General ---
    QString m_name;        ///< Connection name
    Transport m_transport; ///< Network transport

    // --- Network ---
    QUrl m_url; ///< url
    QString m_remoteUserName; ///< Remote user (not needed for LAN)

    // --- Bluetooth ---
    QBluetoothAddress m_bluetoothAddress; ///< Bluetooth address
    QBluetoothUuid m_bluetoothUUID;       ///< Bluetooth UUID

    // QProperty<ConnectionState> m_connectionState; ///< Connection state

    Q_OBJECT_BINDABLE_PROPERTY(ConnectionInfo,
                               ConnectionInfo::ConnectionState,
                               m_connectionState,
                               &ConnectionInfo::connectionStateChanged)
};

#endif // CONNECTIONINFO_HPP
