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

        /*
         * FileTupleColumnBuilder:  Build a tuple of streaming objects.
         *                          One for each member.
         */
        template<typename S, typename T, typename P>
        struct FileTupleColumnBuilder;

        template<typename S, typename T, typename P>
        struct FileTupleColumnBuilder<S, T, std::pair<char const*, P T::*>>
        {
            using ColumnType = typename FileTupleColumn<S, P>::ColumnType;
        };

        /*
         * FileTupleBuilderFromArgs:    Iterate over a tuple to get the stream types.
         */
        template<typename S, typename T, typename TUP>
        struct FileTupleBuilderFromArgs;

        template<typename S, typename T, typename... Args>
        struct FileTupleBuilderFromArgs<S, T, std::tuple<Args...>>
        {
            using FileTuple = std::tuple<typename FileTupleColumnBuilder<S, T, Args>::ColumnType...>;
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

        using FileTuple = typename Impl::FileTupleBuilderFromArgs<S, T, Members>::FileTuple;

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
            bool fail()                             const   {return (state & (eofbit | failbit | badbit)) == (failbit | badbit);}
            operator bool()                         const   {return good() || !fail();}
            bool operator!()                        const   {return !static_cast<bool>(*this);}


            iostate rdstate()                       const   {return state;}
            void setstate(iostate extraState)               {setstateLocalOnly(extraState);setstateSubFiles(extraState, Index{});}
            void clear(iostate newState = goodbit)          {state  = newState;}

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
            void doOpenMembers(std::ios_base::openmode mode, std::index_sequence<I...>);

            template<std::size_t... I>
            void doCloseMembers(std::index_sequence<I...>);

            void open(std::ios_base::openmode mode);
            void setstateLocalOnly(iostate extraState)     {state |= extraState;}

            template<std::size_t... I>
            void setstateSubFiles(iostate extraState, std::index_sequence<I...>);
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
