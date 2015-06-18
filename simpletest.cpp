//
// Created by Leonid on 15.06.2015.
//
#include "big_integer.h"
#include <string>
#include <iostream>

int main()
{
    big_integer a("10000000000000000000000000000000000000000000000000");
    big_integer b("100000000000000000000");
    big_integer c("1000000000000000000000000000000");
    a /= b;
    std::string x = to_string(a);
    std::cout << x << std::endl << (a == c ? "DA" : "NET");
    return 0;
}

