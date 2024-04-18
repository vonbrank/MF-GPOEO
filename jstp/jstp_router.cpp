#include "jstp_router.h"
#include "utils.h"

namespace network
{
    void hello_world(nlohmann::json &request, nlohmann::json &response);

    void JstpRouter::handleRequest(json &request, json &response)
    {
        utils::log("fuck = ");
    }

    void AppRouter::handleRequest(json &request, json &response)
    {
        std::string method = request.at("header").at("method");
        std::string url = request.at("header").at("url");
        // utils::log("url = " + url);

        if (url == "helloworld")
        {
            hello_world(request, response);
        }
    }

    void hello_world(nlohmann::json &request, nlohmann::json &response)
    {
        using json = nlohmann::json;

        response["payload"]["data"] = "Hello world!";
    }
} // namespace network