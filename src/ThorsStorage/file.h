#ifndef THORSANVIL_FileSystem_COLUMNFORMAT_FILE_H
#define THORSANVIL_FileSystem_COLUMNFORMAT_FILE_H

#include <ThorSerialize/Traits.h>
#include <string>
#include <tuple>
#include <fstream>

// See: https://codereview.stackexchange.com/questions/81922/macro-to-build-type-declaration
// For details about the Traits type and how it is built.

namespace ThorsAnvil::FileSystem::ColumnFormat
{

    /*
     * File: The class we want to define.
     *       Forward declaration so it can be used in FileTupleColumn.
     */
    template<typename S, typename T>
    class FileBase;

    namespace Impl
    {
        // Get the type being pointed at by a pointer to member variable.
        template<typename P>
        struct GetPointerMember;

        template<typename R, typename T>
        struct GetPointerMember<std::pair<char const*, R T::*>>
        {
            using ReturnType = R;
        };

        template<typename P>
        using GetPointerMemberType = typename GetPointerMember<P>::ReturnType;

        /*
         * FileTupleColumn: Use template specialization to define the stream class used.
         *                  For basic objects this is `std::fstream`
         *                  For Json::Map types use a FileBase<T> types as this will recursively contain
         *                  File<M> or `std::fstream` types.
         */
        template<typename S, typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct FileTupleColumn;

        template<typename S, typename T>
        struct FileTupleColumn<S, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            using ColumnType  = S;
        };

        template<typename S, typename T>
        struct FileTupleColumn<S, T, ThorsAnvil::Serialize::TraitType::Map>
        {
            using ColumnType  = FileBase<S, T>;
        };

        template<typename S, typename T>
        using FileTupleColumnType = typename FileTupleColumn<S, T>::ColumnType;

        /*
         * FileTupleBuilderFromArgs:    Iterate over a tuple to get the stream types.
         */
        template<typename S, typename T, typename TUP>
        struct FileTupleColumnBuilder;

        template<typename S, typename T, typename... Args>
        struct FileTupleColumnBuilder<S, T, std::tuple<Args...>>
        {
            using FileTuple = std::tuple<FileTupleColumnType<S, GetPointerMemberType<Args>>...>;
        };

        template<typename S, typename T, typename TUP>
        using FileTupleColumnBuilderType = typename FileTupleColumnBuilder<S, T, TUP>::FileTuple;

        /*
         * OpenState
         * Use template recursion to get the state of opening files.
         * Thus we can scan to see if an open would work (and clean up afterwords).
         */
        enum PreOpenState {NoAction, NoDir, DirExists};

        template<typename T, typename TUP = typename ThorsAnvil::Serialize::Traits<T>::Members>
        struct OpenStateBuilder;

        template<typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct OpenStateColumn;

        template<typename T>
        struct OpenStateColumn<T, ThorsAnvil::Serialize::TraitType::Value>
        {
            using OpenState = PreOpenState;
        };

        template<typename T>
        struct OpenStateColumn<T, ThorsAnvil::Serialize::TraitType::Map>
        {
            using OpenState = typename OpenStateBuilder<T>::OpenStateTuple;
        };

        template<typename T>
        using OpenStateColumnType = typename OpenStateColumn<T>::OpenState;

        template<typename T, typename... Args>
        struct OpenStateBuilder<T, std::tuple<Args...>>
        {
            using OpenStateTuple = std::tuple<OpenStateColumnType<GetPointerMemberType<Args>>...>;
        };

        // Holds the open state of a potential scan.

        template<typename T>
        using OpenStateBuilderType = typename OpenStateBuilder<T>::OpenStateTuple;

        template<typename T>
        struct OpenState
        {
            PreOpenState            base;
            OpenStateBuilderType<T> members;
        };

        // File System Functionality
        struct FileSystem
        {
            enum DirResult {DirAlreadyExists, DirCreated, DirFailedToCreate};
            static DirResult makeDirectory(std::string const& path, std::ios_base::openmode mode);
            static bool      isFileOpenable(std::string const& path, std::ios_base::openmode mode);
        };

    }

    template<typename S, typename T>
    class FileBase
    {
        using iostate   = std::ios_base::iostate;
        static constexpr iostate goodbit    = std::ios_base::goodbit;
        static constexpr iostate badbit     = std::ios_base::badbit;
        static constexpr iostate failbit    = std::ios_base::failbit;
        static constexpr iostate eofbit     = std::ios_base::eofbit;
        
        using Traits    = ThorsAnvil::Serialize::Traits<T>;
        using Members   = typename Traits::Members;
        using Index     = std::make_index_sequence<std::tuple_size<Members>::value>;

        using FileTuple = Impl::FileTupleColumnBuilderType<S, T, Members>;

        bool            fileOpened;
        std::string     baseFileName;
        FileTuple       fileTuple;
        iostate         state;

        public:
            FileBase(std::string fileName = "", std::ios_base::openmode mode = std::ios_base::out);

            void open(std::string fileName, std::ios_base::openmode mode = std::ios_base::out);
            void close();

            // https://en.cppreference.com/w/cpp/io/ios_base/iostate
            bool good()                             const   {return !(state & (eofbit | badbit | failbit));}
            bool eof()                              const   {return state & eofbit;}
            bool bad()                              const   {return state & badbit;}
            bool fail()                             const   {return state & (failbit | badbit);}
            operator bool()                         const   {return !fail();}
            bool operator!()                        const   {return !static_cast<bool>(*this);}


            iostate rdstate()                       const   {return state;}
            void setstate(iostate extraState)               {setstateLocalOnly(extraState);setstateSubFiles(extraState, Index{});}
            void clear(iostate newState = goodbit)          {clearLocalOnly(newState);clearSubFiles(newState, Index{});}

            void read(T& data);
            void write(T const& data);

            friend FileBase& operator>>(FileBase& file, T& data)
            {
                file.read(data);
                return file;
            }
            friend FileBase& operator<<(FileBase& file, T const& data)
            {
                file.write(data);
                return file;
            }
        private:
            template<std::size_t... I>
            void readMembers(T& data, std::index_sequence<I...>);

            template<std::size_t... I>
            void writeMembers(T const& data, std::index_sequence<I...>);

            template<std::size_t... I>
            Impl::OpenStateBuilderType<T> doOpenMembersTry(bool& ok, std::ios_base::openmode mode, std::index_sequence<I...>);

            template<std::size_t... I>
            void doOpenMembersFinalize(bool ok, std::ios_base::openmode mode, Impl::OpenStateBuilderType<T> const& state, std::index_sequence<I...>);

            template<std::size_t... I>
            void doCloseMembers(std::index_sequence<I...>);

            void open(std::ios_base::openmode mode);
            void setstateLocalOnly(iostate extraState)      {state |= extraState;}
            void clearLocalOnly(iostate newState)           {state = newState;}

            template<std::size_t... I>
            void setstateSubFiles(iostate extraState, std::index_sequence<I...>);
            template<std::size_t... I>
            void clearSubFiles(iostate newState, std::index_sequence<I...>);

            template<std::size_t I>
            std::string getMemberFilePath();


        public:
            Impl::OpenState<T> doOpenTry(bool& ok, std::string&& fileName, std::ios_base::openmode mode);
            void               doOpenFinalize(bool ok, std::string&& path, std::ios_base::openmode mode, Impl::OpenState<T> const& state);
    };

    template<typename T>
    class IFile: public FileBase<std::ifstream, T>
    {
        public:
            using FileBase<std::ifstream, T>::FileBase;
    };
    template<typename T>
    class OFile: public FileBase<std::ofstream, T>
    {
        public:
            using FileBase<std::ofstream, T>::FileBase;
    };
    template<typename T>
    class File: public FileBase<std::fstream, T>
    {
        public:
            using FileBase<std::fstream, T>::FileBase;
    };

}

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "file.source"
#endif

#ifndef COVERAGE_TEST
#include "file.tpp"
#endif

#endif
