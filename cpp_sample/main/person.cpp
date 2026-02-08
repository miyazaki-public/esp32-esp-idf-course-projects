#include <iostream>
#include <string>
#include "person.h"

Person::Person(std::string name, int age)
{
    _age = age;
    _name = name;
}

void Person::Greet()
{
    std::cout << "Hello I'm " << _name << ". " << "I'm " << _age << " years old." << std::endl;
}

void Person::IncrementAge()
{
    _age++;
}