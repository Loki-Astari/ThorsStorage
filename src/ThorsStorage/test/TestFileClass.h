#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H

#include "gtest/gtest.h"
#include <stdexcept>

static char const   testDataDir[]       = "runTestDir";

static char const   testSecondOpenDir[] = "runTestDir/second";

static char const   simpleTestDir[]     = "runTestDir/PersonData";
static char const   simpleTestIndex[]   = "runTestDir/PersonData/$index";
static char const   simpleTestP1Dir[]   = "runTestDir/PersonData/personOne";
static char const   simpleTestP1Name[]  = "runTestDir/PersonData/personOne/name";
static char const   simpleTestP1NameI[] = "runTestDir/PersonData/personOne/name.index";
static char const   simpleTestP1Age[]   = "runTestDir/PersonData/personOne/age";
static char const   simpleTestP2Dir[]   = "runTestDir/PersonData/personTwo";
static char const   simpleTestP2Name[]  = "runTestDir/PersonData/personTwo/name";
static char const   simpleTestP2NameI[] = "runTestDir/PersonData/personTwo/name.index";
static char const   simpleTestP2Age[]   = "runTestDir/PersonData/personTwo/age";

static char const   lockedTestDir[]     = "runTestDir/lockedDir";
static char const   lockedTestP1Dir[]   = "runTestDir/lockedDir/personOne";
static char const   lockedTestP1Name[]  = "runTestDir/lockedDir/personOne/name";
static char const   lockedTestP1Age[]   = "runTestDir/lockedDir/personOne/age";
static char const   lockedTestP2Dir[]   = "runTestDir/lockedDir/personTwo";
static char const   lockedTestP2Name[]  = "runTestDir/lockedDir/personTwo/name";
static char const   lockedTestP2Age[]   = "runTestDir/lockedDir/personTwo/age";

static char const   lockedFileDir[]     = "runTestDir/openDir";
static char const   lockedFileP1Dir[]   = "runTestDir/openDir/personOne";
static char const   lockedFileP1Name[]  = "runTestDir/openDir/personOne/name";
static char const   lockedFileP1Age[]   = "runTestDir/openDir/personOne/age";
static char const   lockedFileP2Dir[]   = "runTestDir/openDir/personTwo";
static char const   lockedFileP2Name[]  = "runTestDir/openDir/personTwo/name";
static char const   lockedFileP2Age[]   = "runTestDir/openDir/personTwo/age";

// The fixture for testing class Foo.
class TestFileClass: public ::testing::Test
{
    public:
        TestFileClass()
        {
            int checkPreCondition = access(testDataDir, F_OK);
            if (checkPreCondition == 0)
            {
                // Run Time Test Directory exists.
                // A previous test has failed to clean up correctly.
                throw std::runtime_error("Test Directory Detected: It should not be there");
            }
            mkdir(testDataDir, 0'777);
        }
        ~TestFileClass()
        {
            remove(testDataDir);
        }
};

class OpenTwoPeopleTest: public TestFileClass
{
    protected:
        void TearDown() override
        {
            chmod(simpleTestP1Name, 0'777);
            remove(simpleTestP1Name);

            chmod(simpleTestP1NameI, 0'777);
            remove(simpleTestP1NameI);

            chmod(simpleTestP1Age, 0'777);
            remove(simpleTestP1Age);

            chmod(simpleTestP1Dir, 0'777);
            remove(simpleTestP1Dir);

            chmod(simpleTestP2Name, 0'777);
            remove(simpleTestP2Name);

            chmod(simpleTestP2NameI, 0'777);
            remove(simpleTestP2NameI);

            chmod(simpleTestP2Age, 0'777);
            remove(simpleTestP2Age);

            chmod(simpleTestP2Dir, 0'777);
            remove(simpleTestP2Dir);

            chmod(simpleTestIndex, 0'777);
            remove(simpleTestIndex);

            chmod(simpleTestDir, 0'777);
            remove(simpleTestDir);
        }
};

#endif
