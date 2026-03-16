/**
 * @file conncetioninfo.h
 * @brief Provides support for network connections.
 */

#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QObject>
#include <QUrl>

enum class Transport {
    DIRECT,    ///< Direct connection with HTTP protocol
    STUN,      ///< UDP protocol with STUN technology
    BLUETOOTH, ///< BlueTooth protocol
    TURN       ///< Indirect connection with HTTP protocol
};

namespace constants {

constinit const char *DIRECT = "DIRECT";       ///< Direct connection with HTTP protocol
constinit const char *STUN = "STUN";           ///< UDP protocol with STUN technology
constinit const char *BLUETOOTH = "BLUETOOTH"; ///< BlueTooth protocol
constinit const char *TURN = "TURN";           ///< Indirect connection with HTTP protocol

} // namespace constants

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
    QBluetoothAddress m_address; ///< Bluetooth address
    QBluetoothUuid m_uuid;       ///< Bluetooth UUID

    // --- TURN ---
    QString m_userName; ///< Remote user
};

#endif // CONNECTIONINFO_H
