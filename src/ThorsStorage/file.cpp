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

template void FileBase<std::fstream,  Person>::setstateSubFiles<0, 1>(iostate, std::index_sequence<0, 1>);
template void FileBase<std::fstream,  Person>::clearSubFiles<0, 1>(iostate, std::index_sequence<0, 1>);
template void FileBase<std::fstream,  Person>::doCloseMembers<0, 1>(std::index_sequence<0, 1>);
template OpenMemberTuple<Person> FileBase<std::fstream, Person>::doOpenMembersTry(bool& ok, openmode mode, std::index_sequence<0, 1>);
template void FileBase<std::fstream,  Person>::doOpenMembersFinalize(bool ok, openmode mode, OpenMemberTuple<Person> const& state, std::index_sequence<0, 1>);
template void FileBase<std::fstream,  Person>::writeMembers<0, 1>(Person const&, std::index_sequence<0, 1>);
template void FileBase<std::fstream,  Person>::readMembers<0, 1>(Person&, std::index_sequence<0, 1>);

template std::string FileBase<std::fstream, Person>::getMemberFilePath<0>();
template std::string FileBase<std::fstream, Person>::getMemberFilePath<1>();

#endif
