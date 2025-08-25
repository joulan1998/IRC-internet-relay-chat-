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
#include <sstream>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <signal.h>
#define MAX_CLIENT 10

// should be a class
struct client
{
    int fd;
    bool authenticated;
    std::string nickname;
    std::string username;
        // nickname
        // username...
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
        void    socket_options();
        void    bind_server();
        void    set_listen();
        void    handle_new_client(client &local_client);
        void    handle_password(client &local_client);
        // void    handle_message(client &local_client, char *buffer, int i);
        void    handle_message(client &local_client, size_t index);
        void     handle_username(client &local_client);
        // int     handle_username(client &local_client);
        void    handle_nickname(client &local_client/*,int i*/);
        void    free_table(char **table, size_t size);





    // private:
        
};

int string_to_int(const std::string &str);
char	**ft_split(char const *s, char c);
size_t table_size(char **table);
// void show(void *xx)
// {
//     std::cout << xx << std::endl;
// }
#endif