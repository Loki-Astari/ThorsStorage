#include "file.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string_view>
#include <iostream>

namespace ThorsAnvil::FileSystem::ColumnFormat
{

    namespace Impl
    {
        // Get the type being pointed at by a pointer to member variable.
        template<typename P>
        struct GetPointerType;

        template<typename R, typename T>
        struct GetPointerType<std::pair<char const*, R T::*>>
        {
            using ReturnType = R;
        };

        // Read an item from a file.
        // This could be a std::ifstream or IFile
        // Specializations for:
        //      Basic types (Value) as they are fixed size.
        //      std::string:        Also a value type but not fixed size.
        template<typename F, typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct FileReader
        {
            void read(F& file, T& obj)
            {
                file >> obj;
            }
        };

        template<typename T>
        struct FileReader<std::ifstream, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            void read(std::istream& file, T& obj)
            {
                file.read(reinterpret_cast<char*>(&obj), sizeof obj);
            }
        };

        template<>
        struct FileReader<std::ifstream, std::string, ThorsAnvil::Serialize::TraitType::Value>
        {
            void read(std::istream& file, std::string& obj)
            {
                std::getline(file, obj);
                std::transform(std::begin(obj), std::end(obj), std::begin(obj), [](char x){return x == '\0' ? '\n' : x;});
            }
        };

        // Write an item from a file.
        // This could be a std::ofstream or OFile
        // Specializations for:
        //      Basic types (Value) as they are fixed size.
        //      std::string:        Also a value type but not fixed size.
        template<typename F, typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct FileWriter
        {
            void write(F& file, T const& obj)
            {
                file << obj;
            }
        };

        template<typename T>
        struct FileWriter<std::ofstream, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            void write(std::ostream& file, T const& obj)
            {
                file.write(reinterpret_cast<char const*>(&obj), sizeof obj);
            }
        };

        template<>
        struct FileWriter<std::ofstream, std::string, ThorsAnvil::Serialize::TraitType::Value>
        {
            void write(std::ostream& file, std::string const& obj)
            {
                std::string::const_iterator start   = std::begin(obj);
                std::size_t                 used    = 0;

                for (std::size_t size = obj.find('\n'); size != std::string::npos; size = obj.find('\n', size + 1))
                {
                    size = (size == std::string::npos) ? (std::end(obj) - start) : size;
                    std::size_t len = (size - used);
                    file << std::string_view(&*start, size - used) << '\0';

                    start += (len + 1);
                    used  += (len + 1);
                }
                file << std::string_view(&*start) << "\n";
            }
        };
    }


    template<typename S, typename T>
    FileBase<S, T>::FileBase(std::string fileName, std::ios_base::openmode mode)
        : fileOpened(false)
        , baseFileName(std::move(fileName))
        , state(goodbit)
    {
        open(mode);
    }

    template<typename S, typename T>
    void FileBase<S, T>::open(std::string fileName, std::ios_base::openmode mode)
    {
        if (fileOpened)
        {
            return;
        }
        baseFileName = std::move(fileName);
        open(mode);
    }

    template<typename S, typename T>
    void FileBase<S, T>::open(std::ios_base::openmode mode)
    {
        if (baseFileName == "")
        {
            return;
        }
        doOpenMembers(mode, Index{});
        fileOpened = true;
    }

    template<typename S, typename T>
    void FileBase<S, T>::close()
    {
        if (!fileOpened)
        {
            return;
        }
        doCloseMembers(Index{});
        fileOpened = false;
    }

    // ------- SetState Template Recursive --------

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::setstateSubFiles(iostate extraState, std::index_sequence<I...>)
    {
        ([this, extraState]()
        {
            auto& file          = std::get<I>(fileTuple);
            file.setstate(extraState);
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
            auto& file          = std::get<I>(fileTuple);
            file.close();
            setstateLocalOnly(file.rdstate());
        }(), ...);
    }

    // ------- Open Template Recursive --------

    template<typename S, typename T>
    template<std::size_t... I>
    void FileBase<S, T>::doOpenMembers(std::ios_base::openmode mode, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        clear();
        ([this, mode]()
        {
            auto& file          = std::get<I>(fileTuple);
            auto& members       = Traits::getMembers();
            auto& name          = std::get<I>(members).first;

            mkdir(baseFileName.c_str(), 0777);
            std::string     fileNamePath(baseFileName);
            fileNamePath += "/";
            fileNamePath += name;

            file.open(fileNamePath, mode);
            setstateLocalOnly(file.rdstate());
        }(), ...);
        if (!good())
        {
            // If any files failed to open.
            // Then close the ones that did open so we have a consistent state.
            ([this]
            {
                auto& file          = std::get<I>(fileTuple);
                file.close();
            }(), ...);
        }
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
            auto& file          = std::get<I>(fileTuple);
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            using File          = typename std::tuple_element<I, FileTuple>::type;
            using PointerType   = typename std::tuple_element<I, Members>::type;
            using Dst           = typename Impl::GetPointerType<PointerType>::ReturnType;

            Impl::FileReader<File, Dst>    fileReader;
            fileReader.read(file, data.*pointer);
            setstateLocalOnly(file.rdstate());
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
            auto& file          = std::get<I>(fileTuple);
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            using File          = typename std::tuple_element<I, FileTuple>::type;
            using PointerType   = typename std::tuple_element<I, Members>::type;
            using Dst           = typename Impl::GetPointerType<PointerType>::ReturnType;

            Impl::FileWriter<File, Dst>    fileWriter;
            fileWriter.write(file, data.*pointer);
            setstateLocalOnly(file.rdstate());
        }(), ...);
    }

}
