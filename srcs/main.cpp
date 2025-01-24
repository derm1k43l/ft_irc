#include "irc.hpp"
#include "client.hpp"
#include "server.hpp"
#include <iostream>

int main(int ac, char *av[])
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    int port = 0;
    try
    {
        port = std::stoi(av[1]);
        if (port < 1 || port > 65535)
        {
            throw std::out_of_range("Port number must be between 1 and 65535");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    std::string password = av[2];
    if (password.empty())
    {
        std::cerr << "Password cannot be empty\n";
        return 1;
    }

    std::cout << "Port: " << port << " Password: " << password << std::endl;


    return 0;
}