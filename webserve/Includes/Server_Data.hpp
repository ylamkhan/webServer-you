#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Includes/webserver.hpp"
#include "../Includes/Location.hpp"

class Location;
class Client;
typedef std::vector<Location> vecLoc;
class Server {
    private:
        std::string sline;
        std::string root;
        int port_as_int;
        mapErr error_pages;
        std::string server_name;
        unsigned long long client_max_body_size;
        std::string host;
        std::vector<Location> locations;

        bool hostSet;
        bool serverNameSet;
        bool rootSet;
        bool portSet;
        bool maxClientBodySizeSet;
        bool errorPagesSet;
        bool locationsSet;
        std::map<std::string, int> maploc;

    public:
        Server();
        Server(const Server &s);
        Server &operator=(const Server &s);
        ~Server();

        std::string getRoot() const;
        unsigned long long getMaxClientBodySize() const;
        mapErr getErrorPages() const;
        int getPort() const;
        std::string getHost() const;
        std::string getServerNames() const;
        vecLoc getLocations() const;
        std::string getSline() const;
        void setSline(std::string s);

        void setRoot(std::string r);
        void setHost(std::string h);
        void setPort(int p);
        void setServerName(std::string s);
        void setMaxClientBodySize(unsigned long long m);
        void setErrorPages(int code, std::string page);
        void setLocation(Location loc);

        void setrootSet(bool b);
        void setportSet(bool b);
        void setmaxClientBodySizeSet(bool b);
        void seterrorPagesSet(bool b);
        void sethostSet(bool b);
        void setServerNameSet(bool b);
        void setLocationsSet(bool b);

        bool isRootSet() const;
        bool isPortSet() const;
        bool isMaxClientBodySet() const;
        bool isErrorPagesSet() const;
        bool isHostSet() const;
        bool isServerNameSet() const;
        bool isLocationsSet() const;
        
};

#endif