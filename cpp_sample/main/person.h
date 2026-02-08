#pragma once

#include <iostream>
#include <string>

class Person
{
public:
     Person(std::string name, int age);
     void Greet();
     void IncrementAge();
private:
    int _age;
    std::string _name;
};