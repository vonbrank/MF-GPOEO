#ifndef SERVER_H
#define SERVER_H

namespace network
{
    class IServer
    {
    public:
        virtual void run() = 0;
    };

    class IConnectionHandler
    {
    public:
        virtual void handleConnection(int handling_socket) = 0;
    };
} // namespace network

#endif