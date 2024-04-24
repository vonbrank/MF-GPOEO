#ifndef MGO_ROUTER_H
#define MGO_ROUTER_H
#include "nlohmann/json.hpp"

#include "jstp_router.h"
namespace network
{

    class MgoRouter : public JstpRouter
    {
        using json = nlohmann::json;

    public:
        void handleRequest(json& request, json& response) override;
    };
} // namespace network

#endif