#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_PERSON_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_PERSON_H

#include <ThorSerialize/Traits.h>
#include <string>

struct Person
{
    std::string name;
    int         age;
};

ThorsAnvil_MakeTrait(Person, name, age);

#endif
