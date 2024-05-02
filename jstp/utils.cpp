#include <string>
#include <iostream>
#include "utils.h"
#include <mutex>

namespace utils
{
    std::mutex log_mutex;

    void log(const std::string& message)
    {
        std::lock_guard<std::mutex> log_mutex_guard(log_mutex);

        std::cout << message << std::endl;
    }

    void exitWithError(const std::string& errorMessage)
    {
        std::lock_guard<std::mutex> log_mutex_guard(log_mutex);

        log("ERROR: " + errorMessage);
        exit(1);
    }
} // namespace utils
