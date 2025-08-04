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
#include "client.hpp"
#include "channel.hpp"
#define MAX_CLIENT 10

class Channel;

class Server
{
    private:

        std::vector<Channel> channels;
    public:
        int  _port;
        int  _socket_fd;
        std::string _password;
        struct sockaddr_in *_socket_addr;
        std::vector<pollfd> fds;
        std::vector<Client> clients;
        Server(std::string port, std::string password);
        void    start_server();
        void    create_socket();
        void    set_non_clocking();
        void    set_socket_addr();
        void    bind_server();
        void    set_listen();
        void    handle_new_client(Client &local_client, char *buffer, int index);
        void    handle_message(Client &local_client, char *buffer, int i);

        // hachi khas b channel 
        Channel* getchannel(std::string &name_channel);
        void addchannel(std::string &name_channel);
        //hena kanbda l cmd
        void join(int fd, std::vector<std::string> &cmd);
    // private:
        
};

// void show(void *xx)
// {
//     std::cout << xx << std::endl;
// }
#endif