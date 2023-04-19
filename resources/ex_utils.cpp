#include "ex_utils.h"

void exit_errno() {
    std::cerr << "system error: " << strerror(errno) << std::endl; 
    exit(1);
    }