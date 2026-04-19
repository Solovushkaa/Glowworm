#include "downloadinfo.h"

namespace constants {

constinit std::string_view DOWNLOADID = "DOWNLOADID";
constinit std::string_view HOSTKEY = "HOSTKEY";
constinit std::string_view URL = "URL";
constinit std::string_view NAME = "NAME";
constinit std::string_view PATH = "PATH";
constinit std::string_view SAVENAME = "SAVENAME";
constinit std::string_view SAVEPATH = "SAVEPATH";
constinit std::string_view SIZE = "SIZE";
constinit std::string_view LASTRECEIVEDBYTE = "LASTRECEIVEDBYTE";
constinit std::string_view CREATED = "CREATED";
constinit std::string_view MODIFIED = "MODIFIED";
constinit std::string_view ACCESSED = "ACCESSED";
constinit std::string_view DOWNLOADSTATUS = "DOWNLOADSTATUS";

} // namespace constants

const char *getDownloadInfoMemberName(DownloadInfoMember member)
{
    using enum DownloadInfoMember;

    switch (member) {
    case DOWNLOADID:
        return "downloadID";
    case HOSTKEY:
        return "hostKey";
    case URL:
        return "url";
    case NAME:
        return "name";
    case PATH:
        return "path";
    case SAVENAME:
        return "saveName";
    case SAVEPATH:
        return "savePath";
    case SIZE:
        return "size";
    case LASTRECEIVEDBYTE:
        return "lastReceivedByte";
    case CREATED:
        return "created";
    case MODIFIED:
        return "modified";
    case ACCESSED:
        return "accessed";
    case DOWNLOADSTATUS:
        return "downloadStatus";
    default:
        throw BadMemberName{};
    }
}