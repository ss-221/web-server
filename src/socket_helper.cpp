#include <socket_helper.h>

void socket_helper::GetError(const char *exception_message)
{

    char buffer[max_buffer_size];
    if ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    {
        strerror_r(errno, buffer, max_buffer_size);
        std::cout << buffer << std::endl;
    }
    else
    {
        std::cout << strerror_r(errno, buffer, max_buffer_size) << std::endl;
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
    std::cout << "Destroying the socket." << std::endl;
    close(id);
}

int socket_helper::Socket::GetSocket()
{
    return id;
}

void socket_helper::Socket::Close()
{
    close(id);
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
        std::cout << "Improper IP address. Switching to default" << std::endl;
        set_address(INADDR_ANY);
    }
    else if (ret_val < 0)
    {
        GetError("Could not convert the IP address.");
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
    std::cout << "Listening..." << std::endl;

    while (true)
    {
        IncomingSocket client_socket;
        Accept(client_socket);
        std::cout << "Connected to the client." << std::endl;

        std::string message_received = client_socket.Read();
        std::cout << "Message received: " << message_received << std::endl;
        if (message_received.find("QUIT") != std::string::npos)
        {
            std::cout << "Received QUIT. Quitting." << std::endl;
            shutdown(client_socket.GetSocket(), SHUT_RDWR);
            client_socket.Close();
            return;
        }

        client_socket.SendFile("../../website/index.html");
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
    if (len == 0)
        return "";
    std::string message = std::string(buffer, len);

    return message;
}

void socket_helper::IncomingSocket::Send(std::string &message)
{
    if (send(GetSocket(), message.c_str(), message.size(), 0) < 0)
    {
        GetError("Failed to send the message.");
    }
}

void socket_helper::IncomingSocket::SendFile(std::string file_name)
{
    std::string file_extension = std::filesystem::path(file_name).extension();

    int fd = open("../../website/index.html", O_RDONLY);
    if (fd < 0)
    {
        GetError("Failed to open index.html.");
    }

    struct stat file_info;
    fstat(fd, &file_info);
    int total_size = file_info.st_size;
    int block_size = file_info.st_blksize;

    std::string header = "HTTP/1.1 200 OK\r\nContent-Type: ";
    header += GetMimeType(file_extension);
    header += "\r\nContent-Length: ";
    header += std::to_string(total_size);
    header += "\r\n\r\n";
    std::cout << "Header: " << header << '\n';
    Send(header);

    if (fd >= 0)
    {
        while (total_size > 0)
        {
            int bytes_to_send = (total_size < block_size) ? total_size : block_size;
            int bytes_sent = sendfile(GetSocket(), fd, NULL, block_size);
            total_size -= bytes_sent;
            std::cout << "DATA SENT: " << bytes_sent << std::endl;
        }
    }
    close(fd);
}

std::string socket_helper::GetMimeType(std::string &file_extension)
{
    if (file_extension == ".html")
    {
        return "text/html";
    }

    return "";
}
