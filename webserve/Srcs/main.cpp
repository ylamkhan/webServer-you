#include "../Includes/webserver.hpp"
#include "../Includes/Socket.hpp"

int main(int ac, char **av)
{
    if (ac < 3)
    {
        std::string s;
        if (ac == 2)
            s = av[1];
        else
            s = "config/default.yaml";
        ConfigParser config(s);
        try {
            Socket socket(config.getServers());
            socket.handleConnections();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        
    }
    else
    {
        std::cerr<<"Error: invalide argumments...\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}
