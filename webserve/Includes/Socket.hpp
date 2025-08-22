#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <vector>
#include <map>
#include "../Includes/Server_Data.hpp"
#include "../Includes/Client.hpp"

#define  maxEvents  10
class Server;
class Client;

class Socket
{
    private:
        std::vector<Server> servers;
        epoll_event events[maxEvents];
        int epollfd;
        epoll_event event;
        std::vector<int> serverSockets;
        std::map<int, Client> mapClient;
        std::map<int, int> mapServers;

    public:
        Socket &operator=(Socket const &other);
        Socket(Socket const &other);
        Socket(const std::vector<Server>& servers);
        ~Socket();
        void handleConnections();
        void setMapClient(int fd, Client &c);
        std::map<int, Client> getMapClient() const;
};

#endif
