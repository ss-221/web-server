//A very basic client for testing the server
//Accepts the IP and port as args

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

constexpr int max_backlog_queue = 20;
constexpr int max_buffer_size = 200;

void print_error()
{
    char buffer[256];
    std::cout << strerror_r(errno, buffer, 256) << '\n';
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Please enter the args!\n";
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        std::cout << "Cannot create a socket!\n";
        print_error();
        return 0;    
    }

    hostent* server;
    server = gethostbyname(argv[1]);
    if(server == nullptr)
    {
        std::cout << "Server not found!\n";
        return 0;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    memmove(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    server_addr.sin_port = atoi(argv[2]);

    if(connect(sock, (sockaddr * )(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cout << "Failed to connect to the server!\n";
        print_error();
        return 0;
    }

    std::cout << "Enter the message: ";

    std::string msg;
    getline(std::cin, msg);

    if(send(sock, msg.c_str(), msg.size() + 1, 0) < 0)
    {
        std::cout << "Failed to send the message.\n";
        print_error();
    }

    
    char buffer[max_buffer_size];
    memset(buffer, 0, max_buffer_size);
    int len = read(sock, buffer, max_buffer_size);
    if(len < 0)
    {
        std::cout << "Failed to read the message!\n";
        print_error();
        return 0;
    }
    std::cout << "Message received: " << std::string(buffer, len) << '\n';

    close(sock);
    
    return 0;
}