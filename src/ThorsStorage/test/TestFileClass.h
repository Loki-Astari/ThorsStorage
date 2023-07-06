#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H

#include "gtest/gtest.h"
#include "filesystem.h"

using namespace std::string_literals;

static std::string const   testDataDir       = "runTestDir"s;

static std::string const   testSecondOpenDir = "runTestDir/second"s;

static std::string const   simpleIndex   = "/$index"s;
static std::string const   simpleP1Dir   = "/personOne"s;
static std::string const   simpleP1Name  = "/personOne/name"s;
static std::string const   simpleP1NameI = "/personOne/name.index"s;
static std::string const   simpleP1Age   = "/personOne/age"s;
static std::string const   simpleP2Dir   = "/personTwo"s;
static std::string const   simpleP2Name  = "/personTwo/name"s;
static std::string const   simpleP2NameI = "/personTwo/name.index"s;
static std::string const   simpleP2Age   = "/personTwo/age"s;

static std::string const   simpleTestDir = "runTestDir/PersonData"s;
static std::string const   lockedTestDir = "runTestDir/lockedDir"s;
static std::string const   lockedFileDir = "runTestDir/openDir"s;

// The fixture for testing class Foo.
class TestFileClass: public ::testing::Test
{
    public:
        TestFileClass()
        {
            int checkPreCondition = access(testDataDir.c_str(), F_OK);
            if (checkPreCondition == 0)
            {
                // Run Time Test Directory exists.
                // A previous test has failed to clean up correctly.
                throw std::runtime_error("Test Directory Detected: It should not be there");
            }
            namespace FS = std::filesystem;
            FS::create_directories(testDataDir);
        }
        ~TestFileClass()
        {
            namespace FS = std::filesystem;
            FS::remove_all(testDataDir);
        }
};

class TwoPeopleTest: public TestFileClass
{
    std::string fileNameBase;
    protected:
        TwoPeopleTest(std::string const& fileNameBase)
            : fileNameBase(fileNameBase)
        {}
        void TearDown() override
        {
            namespace FS = std::filesystem;
            std::error_code     errorCode;
            FS::permissions(fileNameBase + simpleP1Name, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP1NameI, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP1Age, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP1Dir, FS::perms::all, errorCode);

            FS::permissions(fileNameBase + simpleP2Name, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP2NameI, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP2Age, FS::perms::all, errorCode);
            FS::permissions(fileNameBase + simpleP2Dir, FS::perms::all, errorCode);

            FS::permissions(fileNameBase + simpleIndex, FS::perms::all, errorCode);

            FS::permissions(fileNameBase, FS::perms::all, errorCode);

            // --

            FS::remove_all(fileNameBase);
        }
};

class SimpleTestDir: public TwoPeopleTest
{
    public:
        SimpleTestDir()
            : TwoPeopleTest(simpleTestDir)
        {}
};
class LockedTestDir: public TwoPeopleTest
{
    public:
        LockedTestDir()
            : TwoPeopleTest(lockedTestDir)
        {}
        void SetUp() override
        {
            namespace FS = std::filesystem;
            FS::create_directory(lockedTestDir);
            FS::permissions(lockedTestDir, std::filesystem::perms::none);
        }
};
class LockedFileDir: public TwoPeopleTest
{
    public:
        LockedFileDir()
            : TwoPeopleTest(lockedFileDir)
        {}
        void SetUp() override
        {
            namespace FS = std::filesystem;
            FS::create_directories(lockedFileDir);
            FS::create_directories(lockedFileDir + simpleP2Dir);
            std::ofstream   file(lockedFileDir + simpleP2Name);
            FS::permissions(lockedFileDir + simpleP2Name, std::filesystem::perms::none);
        }
};

#endif
