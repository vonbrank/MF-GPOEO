#include "tcp_server.h"
#include "utils.h"
#include <algorithm>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <sstream>
#include <thread>
#include <unistd.h>

namespace network
{
    TcpServer::TcpServer(std::string ip_address, int port) :
        ip_address(ip_address),
        port(port),
        isServerStarted(false),
        listening_sokcet(),
        socketAddress(),
        socketAddressLen(sizeof(socketAddress))
    {
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_port = htons(port);
        socketAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());
    }

    TcpServer::~TcpServer()
    {
        trycloseServer();
    }

    void TcpServer::run()
    {
        if (init() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(socketAddress.sin_port);
            utils::log(ss.str());
            return;
        }

        startServer();

        trycloseServer();
    }

    int TcpServer::init()
    {
        listening_sokcet = socket(AF_INET, SOCK_STREAM, 0);
        if (listening_sokcet < 0)
        {
            utils::exitWithError("Failed to create listening socket.");
            return 1;
        }

        if (bind(listening_sokcet, (sockaddr*)&socketAddress, socketAddressLen) < 0)
        {
            utils::exitWithError("Failed to connect socket to address");
            return 1;
        }

        isServerSInitialized = true;

        return 0;
    }

    void TcpServer::startServer()
    {
        if (listen(listening_sokcet, 20) < 0)
        {
            utils::exitWithError("Socket listening failed.");
        }

        std::ostringstream ss;
        ss << "[server]: listening on " << inet_ntoa(socketAddress.sin_addr) << ":" << ntohs(socketAddress.sin_port);
        utils::log(ss.str());

        isServerStarted = true;

        while (true)

        {
            int client_socket = accept(listening_sokcet, (sockaddr*)&socketAddress, &socketAddressLen);
            if (client_socket < 0)
            {
                std::ostringstream ss;
                ss << "[server]: failed to accept incomming connection from " << inet_ntoa(socketAddress.sin_addr) << ":" << ntohs(socketAddress.sin_port);
                utils::log(ss.str());
                break;
            }
            else
            {
                std::thread([this, client_socket]()
                            { 
                                auto thread_socket = client_socket; 
                                handleConnection(thread_socket); })
                    .detach();
            }
        }
    }

    void TcpServer::trycloseServer()
    {
        utils::log("Closing TCP Server...\n");

        if (isServerSInitialized)
        {
            shutdown(listening_sokcet, SHUT_RDWR);
            isServerSInitialized = false;
            utils::log("listening_sokcet closed.\n");
        }

        if (isServerStarted)
        {
            isServerStarted = false;
        }
    }

    void TcpServer::handleConnection(int handling_socket)
    {
        for (auto connectionHandler : connectionHandlers)
        {
            connectionHandler->handleConnection(handling_socket);
        }

        close(handling_socket);
    }

    void TcpServer::AddConnectionHandler(IConnectionHandler* connectionHandler)
    {
        if (std::find(connectionHandlers.begin(), connectionHandlers.end(), connectionHandler) == connectionHandlers.end())
        {
            connectionHandlers.push_back(connectionHandler);
        }
    }
    void TcpServer::RemoveConnectionHandler(IConnectionHandler* connectionHandler)
    {
        std::vector<IConnectionHandler*>::iterator it = std::find(connectionHandlers.begin(), connectionHandlers.end(), connectionHandler);
        if (it != connectionHandlers.end())
        {
            connectionHandlers.erase(it);
        }
    }

} // namespace network