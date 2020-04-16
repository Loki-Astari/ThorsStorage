#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using WriteTest = OpenTwoPeopleTest;

TEST_F(WriteTest, ConstructFile)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        ASSERT_TRUE(file);
        file << TwoPeople{Person{"Martin" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    std::string     line;
    int             number;

    std::ifstream   p1nameFile(simpleTestP1Name);
    std::getline(p1nameFile, line);
    ASSERT_EQ(line, "Martin");
    ASSERT_TRUE(p1nameFile);

    std::ifstream   p1ageFile(simpleTestP1Age);
    p1ageFile.read(reinterpret_cast<char*>(&number), sizeof number);
    ASSERT_EQ(number, 20);
    ASSERT_TRUE(p1ageFile);

    std::ifstream   p2nameFile(simpleTestP2Name);
    std::getline(p2nameFile, line);
    ASSERT_EQ(line, "Loki");
    ASSERT_TRUE(p2nameFile);

    std::ifstream   p2ageFile(simpleTestP2Age);
    p2ageFile.read(reinterpret_cast<char*>(&number), sizeof number);
    ASSERT_EQ(number, 22);
    ASSERT_TRUE(p2ageFile);
}

