/**
 * @file conncetion_info.hpp
 * @brief Provides support for network connections.
 */

#ifndef CONNECTIONINFO_HPP
#define CONNECTIONINFO_HPP

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QObject>
#include <QStringView>
#include <QUrl>

enum class Transport {
    DIRECT,    ///< Direct connection with HTTP protocol
    NATTRAV,   ///<
    BLUETOOTH, ///< BlueTooth protocol
    TURN       ///< Indirect connection with HTTP protocol
};

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
    COUNT // To get the size of the enum
};

/**
 * @brief All fields of information about downloads.
 * @param member Enum
 * @return Enum type converted into QStringView
 */
QStringView getConnectionInfoMemberName(ConnectionInfoMember member);

/**
 * @struct ConnectionInfo
 * @brief -
 */
struct ConnectionInfo
{
    // --- General ---
    QString m_name;        ///< Connection name
    Transport m_transport; ///< Network transport

    // --- Network ---
    QUrl m_url; ///< URL
    QString m_remoteUserName; ///< Remote user (not needed for LAN)

    // --- Bluetooth ---
    QBluetoothAddress m_bluetoothAddress; ///< Bluetooth address
    QBluetoothUuid m_bluetoothUUID;       ///< Bluetooth UUID
};

#endif // CONNECTIONINFO_HPP
