#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <>

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
        int         server_socket_;             // Server socket file descriptor
        std::vector<Client*> clients_;          // List of connected clients

        // std::map<int, std::string> client_names_; // Map of client file descriptors to client names

        std::vector<pollfd>  pollfds_;          // List of pollfd structures for poll()
        
        //functions

        void        createServerSocket();
        void        handleNewConnection();
        void        handleClientData(int fd);
        void        handleClientDisconnection(int fd);
        
        
};
