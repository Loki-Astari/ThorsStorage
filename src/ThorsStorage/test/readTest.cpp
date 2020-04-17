#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using ReadTest = OpenTwoPeopleTest;

TEST_F(ReadTest, readTwoPeople)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"Martin" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    // Just construct and destruct
    // Make sure it does not throw.
    TwoPeople               people;
    TC::File<TwoPeople>     file(simpleTestDir);
    ASSERT_TRUE(file);

    file >> people;
    ASSERT_TRUE(file);

    ASSERT_EQ(people.personOne.name, "Martin");
    ASSERT_EQ(people.personOne.age,  20);
    ASSERT_EQ(people.personTwo.name, "Loki");
    ASSERT_EQ(people.personTwo.age,  22);

    TwoPeople               extra;
    file >> extra;
    ASSERT_FALSE(file);
}

