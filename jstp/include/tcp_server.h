#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"

namespace network
{
    class TcpServer : public IServer, public IConnectionHandler
    {
    public:
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
        void run() override;

        void AddConnectionHandler(IConnectionHandler *connectionHandler);
        void RemoveConnectionHandler(IConnectionHandler *connectionHandler);

    private:
        std::string ip_address;
        int port;

        bool isServerSInitialized;
        bool isServerStarted;

        int listening_sokcet;
        sockaddr_in socketAddress;
        unsigned int socketAddressLen;

        std::vector<IConnectionHandler *> connectionHandlers;

    private:
        void startServer();
        void closeServer();
        int init();

        void handleConnection(int handling_socket) override;
    };
} // namespace network

#endif