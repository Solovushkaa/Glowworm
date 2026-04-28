#ifndef UNFINISHEDDOWNLOADDATA_HPP
#define UNFINISHEDDOWNLOADDATA_HPP

#include <QStringView>
#include <array>

namespace test {

inline constexpr QStringView oneDownloadData = uR"json(
{
  "ge07f36174_hostKey_jjk12jjk": {
    "downloadID": "jjk12jjk",
    "downloadState": 2,
    "hostKey": "ge07f36174",
    "url": "192.168.0.4",
    "created": "Fri Mar 28 11:44:50 2026",
    "accessed": "Thu Sep 11 12:46:58 2026",
    "modified": "Fri Mar 28 13:47:50 2026",
    "lastReceivedByte": 41,
    "name": "file.txt",
    "path": "/home/user/",
    "saveName": "file.txt",
    "savePath": "/tmp/DownloadDir/",
    "size": 129324
  }
}
)json";

inline constexpr QStringView threeDownloadData = uR"json(
{
  "6e57f36174_hostKey_8xask982": {
    "downloadID": "8xask982",
    "downloadState": 0,
    "hostKey": "6e57f36174",
    "url": "192.168.0.1",
    "created": "Fri Mar 28 20:47:50 2025",
    "accessed": "Thu Sep 11 21:46:58 2025",
    "modified": "Fri Mar 28 20:47:50 2025",
    "lastReceivedByte": 0,
    "name": "file.txt",
    "path": "/home/user/",
    "saveName": "file.txt",
    "savePath": "/home/user/DownloadDir/",
    "size": 11914
  },
  "6e57f3hh74_hostKey_57f3617f": {
    "downloadID": "57f3617f",
    "downloadState": 2,
    "hostKey": "6e57f3hh74",
    "url": "192.168.0.4",
    "created": "Fri Mar 28 11:44:50 2026",
    "accessed": "Thu Sep 11 12:46:58 2026",
    "modified": "Fri Mar 28 13:47:50 2026",
    "lastReceivedByte": 444,
    "name": "file.txt",
    "path": "/home/user/",
    "saveName": "file.txt",
    "savePath": "/home/user/tt/DownloadDir/",
    "size": 119324
  },
  "ge07f36174_hostKey_jjk12jjk": {
    "downloadID": "jjk12jjk",
    "downloadState": 2,
    "hostKey": "ge07f36174",
    "url": "192.168.0.4",
    "created": "Fri Mar 28 11:44:50 2026",
    "accessed": "Thu Sep 11 12:46:58 2026",
    "modified": "Fri Mar 28 13:47:50 2026",
    "lastReceivedByte": 41,
    "name": "file.txt",
    "path": "/home/user/",
    "saveName": "file.txt",
    "savePath": "/tmp/DownloadDir/",
    "size": 129324
  }
}
)json";

inline constexpr std::array<QStringView, 3> downloadID = {u"8xask982", u"57f3617f", u"jjk12jjk"};
inline constexpr std::array<int, 3> downloadStatus = {0, 0, 0};
inline constexpr std::array<QStringView, 3> hostKey = {u"6e57f36174", u"6e57f3hh74", u"ge07f36174"};
inline constexpr std::array<QStringView, 3> URL = {u"192.168.0.1", u"192.168.0.4", u"192.168.0.4"};
inline constexpr std::array<QStringView, 3> created = {u"Fri Mar 28 20:47:50 2025",
                                                       u"Fri Mar 28 11:44:50 2026",
                                                       u"Fri Mar 28 11:44:50 2026"};
inline constexpr std::array<QStringView, 3> accessed = {u"Thu Sep 11 21:46:58 2025",
                                                        u"Thu Sep 11 12:46:58 2026",
                                                        u"Thu Sep 11 12:46:58 2026"};
inline constexpr std::array<QStringView, 3> modified = {u"Fri Mar 28 20:47:50 2025",
                                                        u"Fri Mar 28 13:47:50 2026",
                                                        u"Fri Mar 28 13:47:50 2026"};
inline constexpr std::array<qint64, 3> lastReceivedByte = {0, 444, 41};
inline constexpr std::array<QStringView, 3> name = {u"file.txt", u"file.txt", u"file.txt"};
inline constexpr std::array<QStringView, 3> path = {u"/home/user/", u"/home/user/", u"/home/user/"};
inline constexpr std::array<QStringView, 3> saveName = {u"file.txt", u"file.txt", u"file.txt"};
inline constexpr std::array<QStringView, 3> savePath = {u"/home/user/DownloadDir/",
                                                        u"/home/user/tt/DownloadDir/",
                                                        u"/tmp/DownloadDir/"};
inline constexpr std::array<qint64, 3> size = {11914, 119324, 129324};

} // namespace test

#endif // UNFINISHEDDOWNLOADDATA_HPP
