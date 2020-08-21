#include "ThorsStorageConfig.h"
#include "file.h"

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "file.tpp"
#include "test/Person.h"

using ThorsAnvil::FileSystem::ColumnFormat::Impl::OpenMemberTuple;
using ThorsAnvil::FileSystem::ColumnFormat::FileBase;
using ThorsAnvil::FileSystem::ColumnFormat::FileMembers;

template FileBase<std::fstream,  TwoPeople>::FileBase(std::string, openmode);
template FileBase<std::ifstream, TwoPeople>::FileBase(std::string, openmode);
template FileBase<std::ofstream, TwoPeople>::FileBase(std::string, openmode);
template FileBase<std::fstream,  Person>::FileBase(std::string, openmode);

template void FileBase<std::fstream,  TwoPeople>::open(std::string, openmode);
template void FileBase<std::ifstream, TwoPeople>::open(std::string, openmode);
template void FileBase<std::ofstream, TwoPeople>::open(std::string, openmode);
template void FileBase<std::fstream,  Person>::open(std::string, openmode);
template void FileBase<std::fstream,  Person>::open(openmode);
template void FileBase<std::fstream,  TwoPeople>::close();
template void FileBase<std::ifstream, TwoPeople>::close();
template void FileBase<std::ofstream, TwoPeople>::close();
template void FileBase<std::fstream,  Person>::close();
template void FileBase<std::ifstream, TwoPeople>::read(TwoPeople&);
template void FileBase<std::fstream,  TwoPeople>::read(TwoPeople&);
template void FileBase<std::fstream,  Person>::read(Person&);
template void FileBase<std::fstream,  Person>::write(Person const&);
template void FileBase<std::ofstream, TwoPeople>::write(TwoPeople const&);
template void FileBase<std::ofstream, TwoPeople>::seekp(streampos);
template void FileBase<std::ofstream, TwoPeople>::seekp(streamoff, seekdir);
template void FileBase<std::ifstream, TwoPeople>::seekg(streampos);
template void FileBase<std::ifstream, TwoPeople>::seekg(streamoff, seekdir);

template void FileMembers<std::fstream,  Person>::setstateMembers<0UL, 1UL>(iostate, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::fstream,  Person>::clearMembers<0UL, 1UL>(iostate, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::fstream,  Person>::doCloseMembers<0UL, 1UL>(std::index_sequence<0UL, 1UL>);
template OpenMemberTuple<Person> FileMembers<std::fstream, Person>::doOpenTryMembers(bool& ok, std::string const& path, openmode mode, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::fstream,  Person>::doOpenFinMembers(bool ok, std::string const& path, openmode mode, OpenMemberTuple<Person> const& state, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::fstream,  Person>::writeMembers<0UL, 1UL>(Person const&, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::fstream,  Person>::readMembers<0UL, 1UL>(Person&, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::ifstream, TwoPeople>::setstateMembers<0UL, 1UL>(iostate, std::index_sequence<0UL, 1UL>);
template void FileMembers<std::ofstream, TwoPeople>::setstateMembers<0UL, 1UL>(iostate, std::index_sequence<0UL, 1UL>);

template std::string FileMembers<std::fstream, Person>::getMemberFilePath<0>(std::string const& path);
template std::string FileMembers<std::fstream, Person>::getMemberFilePath<1>(std::string const& path);


#endif
