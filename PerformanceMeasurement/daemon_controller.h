#ifndef DAEMON_CONTROLLER_H
#define DAEMON_CONTROLLER_H
#include "nlohmann/json.hpp"
#include <string>

namespace daemon_controller
{
    struct HardwareStats
    {
        float cpuPower;
        unsigned int gpuPower;

        float cpuEnery;
        float gpuEnery;

        unsigned int memUsage;
        unsigned int gpuUsage;
    };

    using json = nlohmann::json;

    void handle_time_stamp(std::string description);

    void handle_start();

    void handle_stop();

    void handle_reset(std::string output_path);

    void handle_reset_sm_clock();

    HardwareStats handle_hardware_stats();

} // namespace daemon_controller

#endif