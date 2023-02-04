#include <socket_helper.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

constexpr int max_backlog_queue = 3;

int main(int argc, char *argv[])
{
    try
    {
        using namespace socket_helper;

        ServerSocket server_socket;
        server_socket.set_address(INADDR_ANY);
        server_socket.set_port(atoi(argv[1]));
        server_socket.Bind();

        server_socket.Listen(max_backlog_queue);
        std::cout << "Listening...\n";
    }
    catch (const char *exception_message)
    {
        std::cout << "Exception: " << exception_message << '\n';
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}