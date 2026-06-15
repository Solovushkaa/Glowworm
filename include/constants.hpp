#ifndef ADDITIONAL_HPP
#define ADDITIONAL_HPP

#include <QStandardPaths>
#include <QStringView>

/**
 * @brief Application constants
 */
namespace constants {

// --- DownloadInfo Constants ---
inline constexpr QStringView kDownloadID = u"downloadID";
inline constexpr QStringView kHostkey = u"hostKey";
inline constexpr QStringView kURL = u"url";
inline constexpr QStringView kSaveName = u"saveName";
inline constexpr QStringView kSavePath = u"savePath";
inline constexpr QStringView kLastReceivedByte = u"lastReceivedByte";
inline constexpr QStringView kDownloadState = u"downloadState";

inline constexpr int kDownloadIDLength = 60;

// --- File Constatns ---
inline constexpr QStringView kIsDir = u"isDir";
inline constexpr QStringView kIsReadable = u"isReadable";

// --- Network Constants ---
inline constexpr QStringView kProtocol = u"protocol";
inline constexpr QStringView kTransport = u"transport";
inline constexpr QStringView kConnectionType = u"connectionType";
inline constexpr QStringView kAddress = u"address";
inline constexpr QStringView kPort = u"port";
inline constexpr QStringView kRemoteUserUuid = u"remoteUserUuid";
inline constexpr QStringView kConnectionState = u"connectionState";
inline constexpr QStringView kIsSecureConnection = u"isSecureConnection";

inline constexpr QStringView kMessengerPort = u"messengerPort";
inline constexpr QStringView kTransportPort = u"transportPort";

inline constexpr QStringView TCP = u"TCP";
inline constexpr QStringView UDP = u"UDP";

// --- Universal Constatns ---
inline constexpr QStringView kName = u"name";
inline constexpr QStringView kSize = u"size";
inline constexpr QStringView kPath = u"path";
inline constexpr QStringView kFakePath = u"fakePath";
inline constexpr QStringView kCreated = u"created";
inline constexpr QStringView kModified = u"modified";
inline constexpr QStringView kAccessed = u"accessed";

// --- Settings ---
inline constexpr QStringView kMessengerPortName = u"network/MessengerPort";
inline constexpr QStringView kTransportPortName = u"network/TransportPort";

inline constexpr qint16 kDefaultMessengerPortValue = 6115;
inline constexpr qint16 kDefaultTransportPortValue = 1274;

// --- Dynamic Paths ---
inline const QString kServerCertPath = QStandardPaths::writableLocation(
                                           QStandardPaths::AppDataLocation)
                                       + "/appdata/server/server_cert.crt";
inline const QString kServerKeyPath = QStandardPaths::writableLocation(
                                          QStandardPaths::AppDataLocation)
                                      + "/appdata/server/server_key.key";

} // namespace constants

#endif // ADDITIONAL_HPP
