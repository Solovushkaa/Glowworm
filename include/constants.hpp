#ifndef ADDITIONAL_HPP
#define ADDITIONAL_HPP

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

// --- File Constatns ---
inline constexpr QStringView kIsDir = u"isDir";
inline constexpr QStringView kIsReadable = u"isReadable";

// --- Transport Constants ---
inline constexpr QStringView kProtocol = u"protocol";
inline constexpr QStringView kTransport = u"transport";
inline constexpr QStringView kAddress = u"address";
inline constexpr QStringView kPort = u"port";
inline constexpr QStringView kRemoteUserName = u"remoteUserName";
inline constexpr QStringView kConnectionState = u"connectionState";
inline constexpr QStringView kIsSecureConnection = u"isSecureConnection";

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

} // namespace constants

#endif // ADDITIONAL_HPP
