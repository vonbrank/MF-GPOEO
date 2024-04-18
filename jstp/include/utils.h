#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils
{
    void log(const std::string &message);
    void exitWithError(const std::string &errorMessage);
} // namespace utils

#endif