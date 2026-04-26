#ifndef UNFINISHEDDOWNLOADFORMATER_HPP
#define UNFINISHEDDOWNLOADFORMATER_HPP

#include <QIntegerForSize>
#include <QString>
#include <string_view>
#include <vector>

namespace test {

std::string_view oneDownloadData =
    R"json(
{
  "ge07f36174_hostKey_jjk12jjk": {
    "downloadID": "jjk12jjk",
    "downloadStatus": 2,
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

std::string_view threeDownloadData =
    R"json(
{
  "6e57f36174_hostKey_8xask982": {
    "downloadID": "8xask982",
    "downloadStatus": 0,
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
    "downloadStatus": 2,
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
    "downloadStatus": 2,
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

std::vector<QString> downloadID = {"8xask982", "57f3617f", "jjk12jjk"};
std::vector<int> downloadStatus = {0, 0, 0};
std::vector<QString> hostKey = {"6e57f36174", "6e57f3hh74", "ge07f36174"};
std::vector<QString> URL = {"192.168.0.1", "192.168.0.4", "192.168.0.4"};
std::vector<QString> created = {"Fri Mar 28 20:47:50 2025",
                                "Fri Mar 28 11:44:50 2026",
                                "Fri Mar 28 11:44:50 2026"};
std::vector<QString> accessed = {"Thu Sep 11 21:46:58 2025",
                                 "Thu Sep 11 12:46:58 2026",
                                 "Thu Sep 11 12:46:58 2026"};
std::vector<QString> modified = {"Fri Mar 28 20:47:50 2025",
                                 "Fri Mar 28 13:47:50 2026",
                                 "Fri Mar 28 13:47:50 2026"};
std::vector<qint64> lastReceivedByte = {0, 444, 41};
std::vector<QString> name = {"file.txt", "file.txt", "file.txt"};
std::vector<QString> path = {"/home/user/", "/home/user/", "/home/user/"};
std::vector<QString> saveName = {"file.txt", "file.txt", "file.txt"};
std::vector<QString> savePath = {"/home/user/DownloadDir/",
                                 "/home/user/tt/DownloadDir/",
                                 "/tmp/DownloadDir/"};
std::vector<qint64> size = {11914, 119324, 129324};

} // namespace test

#endif // UNFINISHEDDOWNLOADFORMATER_HPP
