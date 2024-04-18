#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "jstp_router.h"
#include "server.h"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "tcp_server.h"
#include "nlohmann/json.hpp"

namespace network
{
    class JstpServer : public IServer, public IConnectionHandler
    {
        using json = nlohmann::json;

    public:
        JstpServer(std::string ip_address, int port);
        ~JstpServer();

        JstpServer() = delete;
        JstpServer(const JstpServer &) = delete;
        JstpServer(JstpServer &&) = default;

        void run() override;
        void add_router(std::unique_ptr<JstpRouter> router);

    private:
        TcpServer tcpServer;
        std::vector<std::unique_ptr<JstpRouter>> routers;

    private:
        void handleConnection(int handling_socket) override;
        std::string readBytesFromSocket(int handling_socket, size_t numBytes);
        static json defaultResponse();
    };
} // namespace network

#endif