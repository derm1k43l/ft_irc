#include "../inc/server.hpp"

Server::Server(int port, std::string password)
 : port_(port), password_(password), server_socket_(-1)
{
    std::cout << "Server created" << std::endl;
    createServerSocket();
}

Server::Server(Server const& other)
 : port_(other.port_), password_(other.password_), server_socket_(other.server_socket_),
    clients_(other.clients_), pollfds_(other.pollfds_)
{
    std::cout << "Server copied" << std::endl; // we want server to be copied?
}

Server& Server::operator=(Server const& other)
{
    if (this != &other)
    {
        port_ = other.port_;
        password_ = other.password_;
        server_socket_ = other.server_socket_;
        clients_ = other.clients_;
        pollfds_ = other.pollfds_;
    }
    std::cout << "Server assigned" << std::endl;
    return *this;
}

Server::~Server()
{
    std::cout << "Server destroyed" << std::endl;
    stop();
}

void Server::stop()
{
    // close all clients sockets and delete client

    for (auto& client : clients_)
    {
        // close(client->getFd()); // we need a getter for fd in client
        delete client;
    }
    clients_.clear();

    if (server_socket_ != -1)
    {
        close(server_socket_);
        server_socket_ = -1;
    }

    std::cout << "Server stopped" << std::endl;
}

void Server::start()
{
    if (server_socket_ == -1)
    {
        throw std::runtime_error("Server socket not created");
    }

    // add server socket to pollfds_
    pollfd server_pollfd = {server_socket_, POLLIN, 0};
    pollfds_.push_back(server_pollfd);

    std::cout << "Server started" << std::endl;

    while (true)
    {
        int poll_result = poll(pollfds_.data(), pollfds_.size(), -1);
        if (poll_result == -1)
        {
            throw std::runtime_error("Poll error");
        }
        for (auto& pfd : pollfds_)
        {
            if (pfd.revents & POLLIN)
            {
                if (pfd.fd == server_socket_)
                {
                    handleNewConnection();
                }
                else
                {
                    handleClientData(pfd.fd);
                }
            }
        }
    }
}

void Server::createServerSocket()
{
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == -1)
    {
        throw std::runtime_error("Server socket creation error");
    }
    // Set socket options (reuse address)
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(server_socket_);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }

    // Bind socket
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        close(server_socket_);
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }

    // Listen for connections
    if (listen(server_socket_, SOMAXCONN) == -1)
    {
        close(server_socket_);
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }

    std::cout << "Server socket created and listening on port " << port_ << std::endl;
}

void Server::handleNewConnection()
{
    sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_socket_, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1)
    {
        throw std::runtime_error("Failed to accept connection: " + std::string(strerror(errno)));
        return; // or?
    }
    pollfd client_pollfd = {client_fd, POLLIN, 0};
    pollfds_.push_back(client_pollfd);

    // for not we store clein
    clients_.push_back(new Client(client_fd, client_addr));

    std::cout << "New connection handled" << std::endl;
}


void Server::handleClientData(int fd)
{
    char buffer[1024] = {};

    ssize_t bytes_read = recv(fd, buffer, sizeof(buffer), 0);
    if (bytes_read == -1)
    {
        throw std::runtime_error("Failed to read from client: " + std::string(strerror(errno)));
        handleClientDisconnection(fd);
    }

    buffer[bytes_read] = '\0';
    std::string message(buffer);

    std::cout << "Message from client " << fd << ": " << message << std::endl;

    // echo message back to client test
    send(fd, message.c_str(), message.size(), 0);
}

void Server::handleClientDisconnection(int fd)
{
    close(fd);

    // remove client from pollfds_
    for (auto it = pollfds_.begin(); it != pollfds_.end(); it++)
    {
        if (it->fd == fd)
        {
            pollfds_.erase(it);
            break;
        }
    }

    //remove client from clients_
    for (auto it = clients_.begin(); it != clients_.end(); it++)
    {
        if ((*it)->getFd() == fd)
        {
            delete *it;
            clients_.erase(it);
            break;
        }
    }

    std::cout << "Client disconnection handled" << std::endl;
}
