#pragma once
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>



class Client;

class Server
{
    public:
        Server(int port, std::string password);
        Server(Server const& other);
        Server& operator=(Server const& other);
        ~Server();

        void       start();
        void       stop();

    private:
        int         port_;                      // Port number to listen on
        std::string password_;                  // Password to connect to the server
        


};
