#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP

#include "file.h"
#include <iostream>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

namespace ThorsAnvil::FileSystem::ColumnFormat
{

    namespace Impl
    {
        // Open/Read/Write an item from a file.
        // This could be a std::fstream/std::ifstream/std::ofstream or IFile or OFile or IOFile
        //      Basic types (Value) as they are fixed size.
        //      std::string:        Also a value type but not fixed size.
        template<typename F, typename T, ThorsAnvil::Serialize::TraitType type>
        struct FileAccessObject
        {
            F&  file;
            FileAccessObject(F& file)
                : file(file)
            {}

            OpenState<T> openTry(bool& ok, std::string&& path, openmode mode)
            {
                return file.doOpenTry(ok, std::move(path), mode);
            }
            void openFinalize(bool ok, std::string&& path, openmode mode, OpenState<T> const& state)
            {
                file.doOpenFinalize(ok, std::move(path), mode, state);
            }
            void close()                                {file.close();}
            void setstate(iostate extraState)           {file.setstate(extraState);}
            void clear(iostate newState)                {file.clear(newState);}
            iostate rdstate() const                     {return file.rdstate();}
            void read(T& obj)                           {file >> obj;}
            void write(T const& obj) const              {file << obj;}
        };

        template<typename F, typename T>
        struct FileAccessObject<F, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            F&  file;
            FileAccessObject(F& file)
                : file(file)
            {}

            PreOpenState openTry(bool& ok, std::string const& path, openmode mode)
            {
                ok = ok && FileSystem::isFileOpenable(path, mode);
                return NoAction;
            }
            void openFinalize(bool ok, std::string const& path, openmode mode, PreOpenState const&)
            {
                if (ok)
                {
                    file.open(path.c_str(), mode);
                }
            }
            void close()                                {file.close();}
            void setstate(iostate extraState)           {file.setstate(extraState);}
            void clear(iostate newState)                {file.clear(newState);}
            iostate rdstate() const                     {return file.rdstate();}
            void read(T& obj)                           {file.read(reinterpret_cast<char*>(&obj), sizeof obj);}
            void write(T const& obj) const              {file.write(reinterpret_cast<char const*>(&obj), sizeof obj);}
        };

        template<typename F>
        struct FileAccessObject<F, std::string, ThorsAnvil::Serialize::TraitType::Value>
        {
            F&  file;
            FileAccessObject(F& file)
                : file(file)
            {}

            PreOpenState openTry(bool& ok, std::string const& path, openmode mode)
            {
                ok = ok && FileSystem::isFileOpenable(path, mode);
                return NoAction;
            }
            void openFinalize(bool ok, std::string const& path, openmode mode, PreOpenState const&)
            {
                if (ok)
                {
                    file.data.open(path.c_str(), mode);
                    file.index.open((path + ".index").c_str(), mode);
                }
            }
            void close()                                {file.data.close();file.index.close();}
            void setstate(iostate extraState)           {file.data.setstate(extraState);file.index.setstate(extraState);}
            void clear(iostate newState)                {file.data.clear(newState);file.index.clear(newState);}
            iostate rdstate() const                     {return file.data.rdstate() | file.index.rdstate();}
            void read(std::string& obj)
            {
                std::getline(file.data, obj);
                std::transform(std::begin(obj), std::end(obj), std::begin(obj), [](char x){return x == '\0' ? '\n' : x;});

                std::size_t index;
                file.index.read(reinterpret_cast<char*>(&index), sizeof index);
            }
            void write(std::string const& obj)
            {
                std::string::const_iterator start   = std::begin(obj);
                std::size_t                 used    = 0;

                for (std::size_t size = obj.find('\n'); size != std::string::npos; size = obj.find('\n', size + 1))
                {
                    size = (size == std::string::npos) ? (std::end(obj) - start) : size;
                    std::size_t len = (size - used);
                    file.data << std::string_view(&*start, size - used) << '\0';

                    start += (len + 1);
                    used  += (len + 1);
                }
                file.data << std::string_view(&*start) << "\n";
                std::size_t index = file.data.tellp();
                file.index.write(reinterpret_cast<char*>(&index), sizeof index);
            }
        };

        // File System Stuff
        FileSystem::DirResult FileSystem::makeDirectory(std::string const& path, openmode mode)
        {
            using StatusInfo = struct stat;

            StatusInfo        info;
            for (std::size_t pos = path.find('/'); pos != std::string::npos; pos = path.find(pos + 1, '/'))
            {
                std::string     subPath = path.substr(0, pos);
                if ((stat(subPath.c_str(), &info) != 0) && (mkdir(subPath.c_str(), mode) != 0))
                {
                    return DirFailedToCreate;
                }
            }
            if (stat(path.c_str(), &info) == 0)
            {
                return DirAlreadyExists;
            }

            if (mkdir(path.c_str(), mode) == 0)
            {
                return DirCreated;
            }
            return DirFailedToCreate;
        }
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
    }


    template<typename S, typename T>
    FileBase<S, T>::FileBase(std::string fileName, openmode mode)
        : fileOpened(false)
        , baseFileName(std::move(fileName))
        , state(failbit)
    {
        open(mode);
    }

    template<typename S, typename T>
    void FileBase<S, T>::open(std::string fileName, openmode mode)
    {
        if (fileOpened)
        {
            setstate(failbit);
            return;
        }
        baseFileName = std::move(fileName);
        open(mode);
    }

    template<typename S, typename T>
    void FileBase<S, T>::close()
    {
        if (!fileOpened)
        {
            return;
        }
        doCloseMembers(Index{});
        setstateLocalOnly(failbit);
        fileOpened = false;
    }

    // ------- SetState Template Recursive --------

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::setstateSubFiles(iostate extraState, std::index_sequence<I...>)
    {
        ([this, extraState]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.setstate(extraState);
        }(), ...);
    }

    // ------- Clear Template Recursive --------

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::clearSubFiles(iostate newState, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, newState]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.clear(newState);
        }(), ...);
    }

    // ------- Close Template Recursive --------

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::doCloseMembers(std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.close();
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ------- Open Template Recursive --------

    template<typename S, typename T>
    void FileBase<S, T>::open(openmode mode)
    {
        if (baseFileName == "")
        {
            return;
        }
        fileOpened = true;
        clear();

        Impl::OpenState<T> state = doOpenTry(fileOpened, std::move(baseFileName), mode);
        doOpenFinalize(fileOpened, std::move(baseFileName), mode, state);

        if (!fileOpened)
        {
            setstate(failbit);
        }
    }

    template<typename S, typename T>
    Impl::OpenState<T> FileBase<S, T>::doOpenTry(bool& ok, std::string&& fileName, openmode mode)
    {
        Impl::OpenState<T>    result;
        if (!ok)
        {
            result.base = Impl::NoAction;
            return result;
        }

        baseFileName = fileName;
        Impl::FileSystem::DirResult createDir = Impl::FileSystem::makeDirectory(baseFileName, 0'777);

        if (createDir == Impl::FileSystem::DirFailedToCreate)
        {
            ok          = false;
            result.base = Impl::NoAction;
            return result;
        }
        result.base = createDir == Impl::FileSystem::DirAlreadyExists ? Impl::DirExists : Impl::NoDir;
        result.members = doOpenMembersTry(ok, mode, Index{});

        return result;
    }

    template<typename S, typename T>
    void FileBase<S, T>::doOpenFinalize(bool ok, std::string&&, openmode mode, Impl::OpenState<T> const& state)
    {
        if (state.base == Impl::NoAction)
        {
            return;
        }

        doOpenMembersFinalize(ok, mode, state.members, Index{});

        if (!ok && state.base == Impl::NoDir)
        {
            remove(baseFileName.c_str());
        }
    }

    template<typename S, typename T>
    template<std::size_t... I>
    Impl::OpenMemberTuple<T> FileBase<S, T>::doOpenMembersTry(bool& ok, openmode mode, std::index_sequence<I...>)
    {
        Impl::OpenMemberTuple<T> result = std::make_tuple([this, &ok, mode]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));

            auto result = fileAccess.openTry(ok, getMemberFilePath<I>(), mode);
            setstateLocalOnly(fileAccess.rdstate());
            return result;
        }()...);

        return result;
    }

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::doOpenMembersFinalize(bool ok, openmode mode, Impl::OpenMemberTuple<T> const& state, std::index_sequence<I...>)
    {
        ([this, ok, mode, &state]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));

            fileAccess.openFinalize(ok, getMemberFilePath<I>(), mode, std::get<I>(state));
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ------- Read Template Recursive --------

    template<typename S, typename T>
    void FileBase<S, T>::read(T& data)
    {
        if (!good())
        {
            return;
        }
        readMembers(data, Index{});
    }

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::readMembers(T& data, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, &data]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            fileAccess.read(data.*pointer);
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ------- Write Template Recursive --------

    template<typename S, typename T>
    void FileBase<S, T>::write(T const& data)
    {
        if (!good())
        {
            return;
        }
        writeMembers(data, Index{});
    }

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::writeMembers(T const& data, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, &data]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            fileAccess.write(data.*pointer);
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // Get dependant file path

    template<typename S, typename T>
    template<std::size_t I>
    std::string FileBase<S, T>::getMemberFilePath()
    {
        std::string filePath = baseFileName;
        filePath += "/";
        filePath += std::get<I>(Traits::getMembers()).first;

        return filePath;
    }

}

#endif
