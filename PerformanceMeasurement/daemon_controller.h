#ifndef DAEMON_CONTROLLER_H
#define DAEMON_CONTROLLER_H
#include "nlohmann/json.hpp"
#include <string>

namespace daemon_controller
{
    using json = nlohmann::json;

    void handle_time_stamp(std::string description);

    void handle_start();

    void handle_stop();

    void handle_reset(std::string output_path);

    void handle_reset_sm_clock();

} // namespace daemon_controller

#endif