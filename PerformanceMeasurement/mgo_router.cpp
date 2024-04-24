#include "mgo_router.h"
#include "main.h"
#include <iostream>
#include "daemon_controller.h"

namespace network
{
    void MgoRouter::handleRequest(json& request, json& response)
    {
        std::string method = request.at("header").at("method");
        std::string url = request.at("header").at("url");
        auto payload = request.at("payload");

        std::cout << "request json = " << request << std::endl;

        std::stringstream tmpStream;

        if (method == "GET")
        {
            if (url == "hardware-stats")
            {
                pthread_mutex_lock(&lockMsgHandlerSource);

                auto currentCpuPower = PerfData.currCPUPower;

                pthread_mutex_unlock(&lockMsgHandlerSource);

                response["payload"]["data"]["currentCpuPower"] = currentCpuPower;
            }
        }
        else if (method == "POST")
        {
            if (url == "TIME_STAMP")
            {
                std::string description = payload.at("description");
                daemon_controller::handle_time_stamp(description);
            }
            else if (url == "SM_RANGE")
            { // 调节核心频率范围
            }
            else if (url == "SM_RANGE_PCT")
            { // 调节核心频率范围
            }
            else if (url == "RESET_SM_CLOCK")
            { // 重置核心频率范围
                daemon_controller::handle_reset_sm_clock();
            }
            else if (url == "START")
            {
                daemon_controller::handle_start();
            }
            else if (url == "STOP")
            {
                daemon_controller::handle_stop();
            }
            else if (url == "RESET")
            {
                std::string description = payload.at("description");
                daemon_controller::handle_reset(description);
            }
            else {}
        }
    }
} // namespace network