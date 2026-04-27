/**
 * @file conncetioninfo.hpp
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

namespace constants {

inline constexpr QStringView CONNNAME = u"name";
inline constexpr QStringView PROTOCOL = u"protocol";
inline constexpr QStringView TRANSPORT = u"transport";
inline constexpr QStringView ADDRESS = u"address";
inline constexpr QStringView PORT = u"port";

inline constexpr QStringView DIRECT = u"DIRECT";       ///< Direct connection with HTTP protocol
inline constexpr QStringView STUN = u"STUN";           ///< UDP protocol with STUN technology
inline constexpr QStringView BLUETOOTH = u"BLUETOOTH"; ///< BlueTooth protocol
inline constexpr QStringView TURN = u"TURN";           ///< Indirect connection with HTTP protocol

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
    QBluetoothAddress m_bluetoothAddress; ///< Bluetooth address
    QBluetoothUuid m_bluetoothUUID;       ///< Bluetooth UUID

    // --- TURN ---
    QString m_userName; ///< Remote user
};

#endif // CONNECTIONINFO_HPP
