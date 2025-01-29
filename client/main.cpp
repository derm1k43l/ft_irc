#include "client.hpp"

int main() 
{
    int portNum = 1500;
    Client client(portNum);

    if (client.connectToServer()) 
    {
        client.communicate();
    } 
    else 
    {
        std::cout << ">> Error connecting to the server..." << std::endl;
    }
    return 0;
}