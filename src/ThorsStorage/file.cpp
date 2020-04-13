#include "file.h"

#ifdef COVERAGE_TEST
/*
 * This code is only compiled into the unit tests for code coverage purposes
 * It is not part of the live code.
 */
#include "file.tpp"
#include "test/Person.h"

template ThorsAnvil::FileSystem::ColumnFormat::FileBase<std::fstream, Person>::FileBase(std::string, unsigned int);

#endif
