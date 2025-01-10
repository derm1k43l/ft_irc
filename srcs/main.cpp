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
    std::string mode = av[1];

    return 0;
}