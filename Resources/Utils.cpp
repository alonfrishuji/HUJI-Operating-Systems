#include "Utils.h"
#include <iostream>

void exitErr(std::string msg) {
    std::cout << "system error: " << msg << std::endl;
    exit(1);
}