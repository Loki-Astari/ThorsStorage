#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP

#include "file.h"
#include <iostream>
#include <string_view>

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
                file.doOpenFin(ok, std::move(path), mode, state);
            }
            void close()                                {file.close();}
            void setstate(iostate extraState)           {file.setstate(extraState);}
            void clear(iostate newState)                {file.clear(newState);}
            iostate rdstate() const                     {return file.rdstate();}
            void read(T& obj)                           {file.read(obj);}
            void write(T const& obj) const              {file.write(obj);}
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
    }

    // ==== FileMembers ====

    template<typename S, typename T>
    FileMembers<S, T>::FileMembers()
        : state(failbit)
    {}

    // ---- Open ----

    template<typename S, typename T>
    Impl::OpenState<T> FileMembers<S, T>::doOpenTry(bool& ok, std::string const& path, openmode mode)
    {
        Impl::OpenState<T>  result;
        if (!ok)
        {
            result.base = Impl::NoAction;
            return result;
        }

        FileSystem::DirResult createDir = FileSystem::makeDirectory(path, 0'777);

        if (createDir == FileSystem::DirFailedToCreate)
        {
            ok          = false;
            result.base = Impl::NoAction;
            return result;
        }

        result.base = createDir == FileSystem::DirAlreadyExists ? Impl::DirExists : Impl::NoDir;
        result.members = doOpenTryMembers(ok, path, mode, Index{});
        return result;
    }

    template<typename S, typename T>
    template<std::size_t... I>
    Impl::OpenMemberTuple<T> FileMembers<S, T>::doOpenTryMembers(bool& ok, std::string const& path, openmode mode, std::index_sequence<I...>)
    {
        Impl::OpenMemberTuple<T> result = std::make_tuple([this, &ok, &path, mode]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));

            auto result = fileAccess.openTry(ok, getMemberFilePath<I>(path), mode);
            setstateLocalOnly(fileAccess.rdstate());
            return result;
        }()...);

        return result;
    }

    template<typename S, typename T>
    void FileMembers<S, T>::doOpenFin(bool ok, std::string const& path, openmode mode, Impl::OpenState<T> const& state)
    {
        if (state.base == Impl::NoAction)
        {
            return;
        }

        doOpenFinMembers(ok, path,  mode, state.members, Index{});

        if (!ok && state.base == Impl::NoDir)
        {
            remove(path.c_str());
        }
    }

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::doOpenFinMembers(bool ok, std::string const& path, openmode mode, Impl::OpenMemberTuple<T> const& state, std::index_sequence<I...>)
    {
        ([this, ok, &path, mode, &state]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));

            fileAccess.openFinalize(ok, getMemberFilePath<I>(path), mode, std::get<I>(state));
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ---- Close ----

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::doCloseMembers(std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.close();
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ---- Read/Write ----

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::readMembers(T& data, std::index_sequence<I...>)
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

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::writeMembers(T const& data, std::index_sequence<I...>)
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

    // ---- Clear State Bits ----

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::clearMembers(iostate newState, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, newState]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.clear(newState);
        }(), ...);
    }

    // ---- Set State Bits ----

    template<typename S, typename T>
    template<std::size_t... I>
    void FileMembers<S, T>::setstateMembers(iostate extraState, std::index_sequence<I...>)
    {
        ([this, extraState]()
        {
            FileAccessObject<I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.setstate(extraState);
        }(), ...);
    }

    // ---- Get Index Element Path Name from base ----

    template<typename S, typename T>
    template<std::size_t I>
    std::string FileMembers<S, T>::getMemberFilePath(std::string const& path)
    {
        std::string filePath = path;
        filePath += "/";
        filePath += std::get<I>(Traits::getMembers()).first;

        return filePath;
    }

    // ===== FileBase =========

    template<typename S, typename T>
    FileBase<S, T>::FileBase(std::string fileName, openmode mode)
        : fileOpened(false)
        , baseFileName(std::move(fileName))
    {
        open(mode);
    }

    // ---- Open ----
    // Open is complex:
    //  Only the first function here is public.
    //  The second is the main entry point called by the public open and the constructor.
    //  It performs the open in two two stages:
    //      Stage 1:    doOpenTry:
    //                      Create Directory if they don't exist.
    //                      Check if required files can be opened in the required mode in a directory.
    //      Stage 2:    doOpenFin:
    //                      If all files can be created then create all files.
    //                      If we can not create all the files then remove the directories we created in stage 1.

    template<typename S, typename T>
    void FileBase<S, T>::open(std::string fileName, openmode mode)
    {
        if (fileOpened)
        {
            FileMembers<S, T>::setstate(failbit);
            return;
        }
        baseFileName = std::move(fileName);
        open(mode);
    }

    template<typename S, typename T>
    void FileBase<S, T>::open(openmode mode)
    {
        if (baseFileName == "")
        {
            return;
        }
        fileOpened = true;
        FileMembers<S, T>::clear();

        Impl::OpenState<T> state = FileMembers<S, T>::doOpenTry(fileOpened, baseFileName, mode);
        FileMembers<S, T>::doOpenFin(fileOpened, baseFileName, mode, state);

        if (!fileOpened)
        {
            FileMembers<S, T>::setstate(failbit);
        }
    }

    // ---- close ----

    template<typename S, typename T>
    void FileBase<S, T>::close()
    {
        if (!fileOpened)
        {
            return;
        }
        FileMembers<S, T>::close();
        FileBase<S, T>::setstateLocalOnly(failbit);
        fileOpened = false;
    }

    // ---- read/write ----

    template<typename S, typename T>
    void FileBase<S, T>::read(T& data)
    {
        if (!FileMembers<S, T>::good())
        {
            return;
        }
        FileMembers<S, T>::read(data);
    }

    template<typename S, typename T>
    void FileBase<S, T>::write(T const& data)
    {
        if (!FileMembers<S, T>::good())
        {
            return;
        }
        FileMembers<S, T>::write(data);
    }

}

#endif
