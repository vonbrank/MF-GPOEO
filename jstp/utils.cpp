#include <string>
#include <iostream>
#include "utils.h"

namespace utils
{
    void log(const std::string &message)
    {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string &errorMessage)
    {
        log("ERROR: " + errorMessage);
        exit(1);
    }
} // namespace utils
