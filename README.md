# ThorsStorage

[![Build Status](https://travis-ci.org/Loki-Astari/ThorsStorage.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsStorage)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=240341)](http://codereview.stackexchange.com/q/240341/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=240825)](http://codereview.stackexchange.com/q/240825/507)

![ThorStream](img/storage.jpg)

This library depends on files in [ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer) so you must have this installed first. The easiest way to get ThorsSerializer and this project is simply to install the brew library `thors-serializer`.

This library provides a simple way to save a structure data into multiple files. Each file representing a specific field in the structure. This allows a very efficient scan as most data has a fixed size and variable sized data has a specific index created for the column file.

###Example

````c++
class Team {
    int id;
    int size;
};
class Person {
    std::string   name;
    int           age;
    double        salary;
    Team.         team;
};

ThorsAnvil_MakeTrait(Team, id, size);
ThorsAnvil_MakeTrait(Person, name, age, salary, team);

int main()
{
    CF::File<Person>    saveFile("data");
    saveFile << Person{"Loki", 33, 12345, Team{12,34}};
    saveFile << Person{"Tom",  25, 34566, Team{13,34}};
    saveFile << Person{"Luke", 45, 43125, Team{14,34}};
    saveFile << Person{"John", 32, 43895, Team{15,34}};
    saveFile << Person{"Pete", 18, 43111, Team{16,34}};
}
````


## HomeBrew

Can be installed via brew on Mac and Linux

    brew install thors-serializer

* Mac: https://formulae.brew.sh/formula/thors-serializer
* Linux: https://formulae.brew.sh/formula-linux/thors-serializer

## Header Only

To install header only version

    git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsSerializer.git
    git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsStorage.git

## Contributors

Added the all-contributers bot to generate the table.


