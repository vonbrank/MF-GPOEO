#include "jstp_server.h"
#include "utils.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include "nlohmann/json.hpp"
#include "jstp_router.h"

namespace network
{
    JstpServer::JstpServer(std::string ip_address, int port) :
        tcpServer(TcpServer(ip_address, port))
    {
        tcpServer.AddConnectionHandler(this);
    }

    JstpServer::~JstpServer()
    {
        tcpServer.RemoveConnectionHandler(this);
    }

    void JstpServer::run()
    {
        tcpServer.run();
    }
    void JstpServer::handleConnection(int handling_socket)
    {
        utils::log("[jstp-server] Handling connection...");

        std::stringstream ss_log;

        std::stringstream ss_first_line;
        std::string ch;
        while (true)
        {
            ch = readBytesFromSocket(handling_socket, 1);
            if (ch.size() == 0)
            {
                return;
            }
            ss_first_line << ch;
            if (ss_first_line.str().find("\r\n") != std::string::npos)
            {
                break;
            }
        }
        std::string first_line_str = ss_first_line.str();
        first_line_str = first_line_str.substr(0, first_line_str.size() - 2);
        int json_string_length;
        try
        {
            json_string_length = std::stoi(first_line_str);
        }
        catch (const std::invalid_argument &e)
        {
            ss_log.clear();
            ss_log << "无效参数: " << e.what() << std::endl;
            utils::log(ss_log.str());
            return;
        }
        catch (const std::out_of_range &e)
        {
            ss_log.clear();
            ss_log << "范围超出: " << e.what() << std::endl;
            utils::log(ss_log.str());
            return;
        }

        // ss_log.clear();
        // ss_log << "request json length = " << std::to_string(json_string_length) << std::endl;
        // utils::log(ss_log.str());

        std::string json_message_string = readBytesFromSocket(handling_socket, json_string_length);
        // ss_log.clear();
        // ss_log << "request json = " << json_message_string << std::endl;
        // utils::log(ss_log.str());

        using json = nlohmann::json;
        auto jstp_request = json::parse(json_message_string.c_str());

        json jstp_response = defaultResponse();

        // utils::log("routers length = " + std::to_string(routers.size()));

        for (auto &router : routers)
        {
            router->handleRequest(jstp_request, jstp_response);
        }

        std::string jstp_response_string_payload = jstp_response.dump();

        std::stringstream ss_jstp_response_string;
        size_t jstp_response_string_length = jstp_response_string_payload.length();
        ss_jstp_response_string << std::to_string(jstp_response_string_length) << "\r\n"
                                << jstp_response_string_payload;
        std::string jstp_response_string = ss_jstp_response_string.str();

        ss_log.clear();
        ss_log << "response jstp message: \n"
               << ss_jstp_response_string.str() << std::endl;
        utils::log(ss_log.str());

        int bytesWritten = write(handling_socket, jstp_response_string.data(), jstp_response_string.length());
        if (bytesWritten != jstp_response_string.length())
        {
            utils::log("[jstp-server] Failed to sending response to client.");
        }

        utils::log("[jstp-server] Connection closed...");
    }

    std::string JstpServer::readBytesFromSocket(int handling_socket, size_t numBytes)
    {
        size_t BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        size_t totalBytesRead = 0;
        std::ostringstream ss_data;

        while (totalBytesRead < numBytes)
        {
            int bytesRead = recv(handling_socket, buffer, std::min(BUFFER_SIZE - 1, numBytes - totalBytesRead), 0);
            if (bytesRead < 0)
            {
                utils::log("Failed to receive bytes from client socket connection.");
                break;
            }
            else if (bytesRead == 0)
            {
                if (totalBytesRead)
                {
                    utils::log("Connection has been closed before finishing reading.");
                }
                break;
            }
            totalBytesRead += bytesRead;
            buffer[bytesRead] = '\0';
            ss_data << buffer;
        }

        return ss_data.str();
    }

    nlohmann::json JstpServer::defaultResponse()
    {
        using json = nlohmann::json;

        json response;
        response["header"] = {{"status", 200}};
        response["payload"] = nullptr;

        return response;
    }

    void JstpServer::add_router(std::unique_ptr<JstpRouter> router)
    {
        routers.push_back(std::move(router));
    }

} // namespace network