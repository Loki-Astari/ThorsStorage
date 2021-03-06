#include "ThorsStorageConfig.h"
#include "filesystem.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

using namespace ThorsAnvil::FileSystem;

// File System Stuff
HEADER_ONLY_INCLUDE
FileSystem::DirResult FileSystem::makeDirectory(std::string const& path, mode_t permissions)
{
    using StatusInfo = struct stat;

    StatusInfo        info;
    for (std::size_t pos = path.find('/'); pos != std::string::npos; pos = path.find(pos + 1, '/'))
    {
        std::string     subPath = path.substr(0, pos);
        if ((stat(subPath.c_str(), &info) != 0) && (mkdir(subPath.c_str(), permissions) != 0))
        {
            return DirFailedToCreate;
        }
    }
    if (stat(path.c_str(), &info) == 0)
    {
        return DirAlreadyExists;
    }

    if (mkdir(path.c_str(), permissions) == 0)
    {
        return DirCreated;
    }
    return DirFailedToCreate;
}

HEADER_ONLY_INCLUDE
bool FileSystem::isFileOpenable(std::string const& path, openmode mode)
{
    bool result = true;
    int accessFlag = ((mode & in) ? R_OK : 0)
                   | ((mode & out)? W_OK : 0);
    if (access(path.c_str(), accessFlag) != 0)
    {
        // This is still OK if we want to open a file for writing as we will be creating it.
        // But to make sure we have permission we have to check three things.
        //  1: The errors for accesses is because the file does not exist.
        //  2: We want to open the file for writing.
        //  3: The directory we want to open the file is writable by this processes.
        //
        //  Otherwise the file is not open-able for the mode we want.
        result = (errno == ENOENT) && (mode & out) && (access(path.substr(0, path.find_last_of('/')).c_str(), W_OK) == 0);
    }
    return result;
}

HEADER_ONLY_INCLUDE
bool FileSystem::removeFileOrDirectory(std::string const& path)
{
    int state = remove(path.c_str());
    return state == 0;
}
