#include <socket_helper.h>
#include <iostream>

void socket_helper::GetError(const char *exception_message)
{

    char buffer[max_buffer_size];
    if ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    {
        strerror_r(errno, buffer, max_buffer_size);
        std::cout << buffer << '\n';
    }
    else
    {
        std::cout << strerror_r(errno, buffer, max_buffer_size) << '\n';
    }
    throw(exception_message);
}

socket_helper::ServerSocket::ServerSocket(int domain_, int type_, int protocol_) : domain(domain_), type(type_), protocol(protocol_)
{
    id = socket(domain, type, protocol);
    if (id < 0)
    {
        GetError("Socket creation failed.");
    }
    address.sin_family = AF_INET;
};

socket_helper::ServerSocket::~ServerSocket()
{
    std::cout << "Destroying the socket.\n";
    close(id);
}

int socket_helper::Socket::GetSocket()
{
    return id;
}

void socket_helper::ServerSocket::set_address(in_addr_t addr)
{
    address.sin_addr.s_addr = addr;
}

void socket_helper::ServerSocket::set_address(std::string addr)
{
    int ret_val = inet_pton(domain, addr.c_str(), &(address.sin_addr));
    if (ret_val == 0)
    {
        std::cout << "Improper IP address.\n";
    }
    else if (ret_val < 0)
    {
        GetError("Could not convert IP address.");
    }
}

sockaddr_in *socket_helper::Socket::get_address()
{
    return &address;
}

void socket_helper::Socket::set_port(in_port_t port)
{
    address.sin_port = port;
}

void socket_helper::ServerSocket::Bind()
{
    if (bind(id, (sockaddr *)(&address), sizeof(address)) < 0)
    {
        GetError("Failed to bind the address to the socket.");
    }
}

void socket_helper::ServerSocket::Listen(int backlog)
{
    if (listen(id, backlog) < 0)
    {
        GetError("Failed to listen.");
    }
    std::cout << "Listening...\n";

    while (true)
    {
        IncomingSocket client_socket;
        Accept(client_socket);

        std::cout << "Connected to the client.\n";

        std::string message_received = client_socket.Read();
        std::cout << "Message received: " << message_received << '\n';

        std::string msg = "Hello to you too!";

        if (send(client_socket.GetSocket(), msg.c_str(), msg.size() + 1, 0) < 0)
        {
            GetError("Failed to send the message.");
        }

        if (message_received == "quit")
        {
            std::cout << "Received quit. Quitting.\n";
            return;
        }
    }
}

void socket_helper::ServerSocket::Accept(IncomingSocket &client_socket)
{
    sockaddr_in *client_sock_addr = client_socket.get_address();
    socklen_t addr_len = sizeof(*client_sock_addr);
    client_socket = accept(id, (sockaddr *)(client_sock_addr), &addr_len);

    if (client_socket.GetSocket() < 0)
    {
        GetError("Failed to accept the incoming socket.");
    }
}

void socket_helper::IncomingSocket::operator=(const int &sock)
{
    id = sock;
}

void socket_helper::IncomingSocket::set_max_read_bytes(size_t bytes)
{
    max_read_bytes = bytes;
}

std::string socket_helper::IncomingSocket::Read()
{
    char buffer[max_buffer_size];
    int len = read(id, buffer, max_buffer_size);
    if (len < 0)
    {
        GetError("Failed to read the message.");
    }

    std::string message = std::string(buffer, len);
    message.pop_back();

    return message;
}
