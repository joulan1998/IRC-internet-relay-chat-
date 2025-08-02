#ifndef IRCSERV
#define IRCSERV
#include <string>
#include <iostream>
#include <exception>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <algorithm>
#define MAX_CLIENT 10

struct client
{
    int fd;
    bool authenticated;
};

class server
{
    public:
        int  _port;
        int  _socket_fd;
        std::string _password;
        struct sockaddr_in *_socket_addr;
        std::vector<pollfd> fds;
        std::vector<client> clients;




        server(std::string port, std::string password);
        void    start_server();
        void    create_socket();
        void    set_non_clocking();
        void    set_socket_addr();
        void    bind_server();
        void    set_listen();
        void    handle_new_client(client &local_client, char *buffer, int index);
        void    handle_message(client &local_client, char *buffer, int i);





    // private:
        
};

// void show(void *xx)
// {
//     std::cout << xx << std::endl;
// }
#endif