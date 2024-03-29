#include "filesystem.h"

using namespace ThorsAnvil::FileSystem;

// File System Stuff
THORS_STORAGE_HEADER_ONLY_INCLUDE
FileSystem::DirResult FileSystem::makeDirectory(std::string const& path, mode_t permissions)
{
    namespace FS = std::filesystem;
    std::error_code error;
    if (FS::exists(path, error))
    {
        return DirAlreadyExists;
    }
    if (FS::create_directories(path, error))
    {
        FS::permissions(path, static_cast<FS::perms>(permissions));
        return DirCreated;
    }
    return DirFailedToCreate;
}

THORS_STORAGE_HEADER_ONLY_INCLUDE
bool FileSystem::isFileOpenable(std::string const& path, openmode mode)
{
    namespace FS = std::filesystem;
    static constexpr int readPerm  = (static_cast<int>(FS::perms::owner_read) | static_cast<int>(FS::perms::group_read) | static_cast<int>(FS::perms::others_read));
    static constexpr int writePerm = (static_cast<int>(FS::perms::owner_write) | static_cast<int>(FS::perms::group_write) | static_cast<int>(FS::perms::others_write));

    std::error_code     errorCode;
    FS::file_status stat = FS::status(path, errorCode);

    bool    result = true;
    if (mode & in)
    {
        result =
            // To read the file must exist.
            (stat.type() != FS::file_type::not_found)
            // And be readable by one of the groups (not perfect but reasonable)
         && (static_cast<int>(stat.permissions()) & readPerm);
    }
    if (mode & out)
    {
        result =
            // File does not exist but directory has write permission.
            (stat.type() == FS::file_type::not_found && isFileOpenable(path.substr(0, path.find_last_of('/')), out))
            // Or file exists and has permission
         || ((stat.type() != FS::file_type::not_found) && (static_cast<int>(stat.permissions()) & writePerm));
    }
    return result;
}

THORS_STORAGE_HEADER_ONLY_INCLUDE
bool FileSystem::removeFileOrDirectory(std::string const& path)
{
    int state = remove(path.c_str());
    return state == 0;
}
