#include "gtest/gtest.h"

#include "file.tpp"
#include "test/Person.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;


TEST(Open, Constructor)
{
    // Just construct and destruct
    TC::File<Person>    file;
}

