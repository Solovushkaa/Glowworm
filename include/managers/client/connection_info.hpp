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
    STUN,      ///< UDP protocol with STUN technology
    BLUETOOTH, ///< BlueTooth protocol
    TURN       ///< Indirect connection with HTTP protocol
};

/**
 * @brief All fields of information about connections.
 */
enum class ConnectionInfoMember {
    Name,
    Transport,
    URL,
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

    // --- Direct, STUN, TURN ---
    QUrl m_url; ///< URL

    // --- Bluetooth ---
    QBluetoothAddress m_bluetoothAddress; ///< Bluetooth address
    QBluetoothUuid m_bluetoothUUID;       ///< Bluetooth UUID

    // --- TURN ---
    QString m_remoteUserName; ///< Remote user
};

#endif // CONNECTIONINFO_HPP
