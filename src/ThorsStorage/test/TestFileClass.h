#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H

#include "gtest/gtest.h"

static char const   testDataDir[]       = "runTestDir";

static char const   simpleTestDir[]     = "runTestDir/PersonData";
static char const   simpleTestP1Dir[]   = "runTestDir/PersonData/personOne";
static char const   simpleTestP1Name[]  = "runTestDir/PersonData/personOne/name";
static char const   simpleTestP1Age[]   = "runTestDir/PersonData/personOne/age";
static char const   simpleTestP2Dir[]   = "runTestDir/PersonData/personTwo";
static char const   simpleTestP2Name[]  = "runTestDir/PersonData/personTwo/name";
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
            remove(simpleTestP1Name);
            remove(simpleTestP1Age);
            remove(simpleTestP1Dir);
            remove(simpleTestP2Name);
            remove(simpleTestP2Age);
            remove(simpleTestP2Dir);
            remove(simpleTestDir);
        }
};

#endif
