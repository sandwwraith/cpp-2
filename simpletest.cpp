//
// Created by Leonid on 15.06.2015.
//
#include "big_integer.h"
#include <string>
#include <iostream>
#include "vector_storage.h"

int main()
{
    /*big_integer a("23283064360");
    big_integer b("100000000000000000000");
    big_integer c("1000000000000000000000000000000");
    a <<= 32;
    std::string x = to_string(a);
    std::cout << x << std::endl;*/
    vector_storage aa;
    big_integer a("1078");
    for (size_t i = 0; i < 20; i++)
    {
        aa.push_back(big_integer(100 + i));
    }
    for (size_t i = 0; i < 20; i++)
    {
        std::cout << "i: " << to_string(aa[19 - i]) << std::endl;
        //aa.pop_back();
    }
    vector_storage bb(aa);
    std::cout << "CLEARING.....\n";
    aa.clear();
    for (size_t i = 0; i < 20; i++)
    {
        aa.push_back(big_integer(i));
    }

    aa.erase(aa.begin() + 2, aa.begin() + 10);
    for (big_integer *i = aa.begin(); i < aa.end(); i++)
    {
        std::cout << to_string(*i) << std::endl;
    }
    std::cout << "COPY: \n";
    for (big_integer *i = bb.begin(); i < bb.end(); i++)
    {
        std::cout << to_string(*i) << std::endl;
    }

    return 0;
}

