#include "file.h"

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "file.tpp"
#include "test/Person.h"

using ThorsAnvil::FileSystem::ColumnFormat::Impl::OpenStateTuple;
using ThorsAnvil::FileSystem::ColumnFormat::FileBase;

template FileBase<std::fstream, Person>::FileBase(std::string, std::ios_base::openmode);

template void FileBase<std::fstream, Person>::open(std::string, std::ios_base::openmode);
template void FileBase<std::fstream, Person>::open(std::ios_base::openmode);
template void FileBase<std::fstream, Person>::close();
template void FileBase<std::fstream, Person>::read(Person&);
template void FileBase<std::fstream, Person>::write(Person const&);

template void FileBase<std::fstream, Person>::setstateSubFiles<0, 1>(iostate, std::index_sequence<0, 1>);
template void FileBase<std::fstream, Person>::clearSubFiles<0, 1>(iostate, std::index_sequence<0, 1>);
template void FileBase<std::fstream, Person>::doCloseMembers<0, 1>(std::index_sequence<0, 1>);
template OpenStateTuple<Person> FileBase<std::fstream, Person>::doOpenMembersTry(bool& ok, std::ios_base::openmode mode, std::index_sequence<0, 1>);
template void FileBase<std::fstream, Person>::doOpenMembersFinalize(bool ok, std::ios_base::openmode mode, OpenStateTuple<Person> const& state, std::index_sequence<0, 1>);
template void FileBase<std::fstream, Person>::writeMembers<0, 1>(Person const&, std::index_sequence<0, 1>);
template void FileBase<std::fstream, Person>::readMembers<0, 1>(Person&, std::index_sequence<0, 1>);

template std::string FileBase<std::fstream, Person>::getMemberFilePath<0>();
template std::string FileBase<std::fstream, Person>::getMemberFilePath<1>();


template FileBase<std::fstream, TwoPeople>::FileBase(std::string, unsigned int);


#endif
