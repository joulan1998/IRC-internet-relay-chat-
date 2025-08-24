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
class Client;

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
        void    set_non_blocking();
        void    set_socket_addr();
        void    socket_options();
        void    bind_server();
        void    set_listen();
        void    handle_new_client(Client &local_client, std::string value, size_t index);
        // void    handle_new_client(Client &local_client);
        // void    handle_password(Client &local_client);
        void    handle_password(Client &local_client, std::string value/*, size_t index*/);
        void    handle_message(Client &local_client, char *buffer, int i);
        void    handle_message(Client &local_client, size_t index);
        void     handle_username(Client &local_client, std::string value);
        // void     handle_username(Client &local_client);
        // int     handle_username(client &local_client);
        // void    handle_nickname(Client &local_client/*,int i*/);
        void    handle_nickname(Client &local_client, std::string value);

    
        //send error
        void print_error(int fd, std::string msg);

        // hachi khas b channel 
        Channel* getchannel(const std::string &name_channel);
        int addchannel(Client _client, const std::string &name_channel);
        //hena kanbda l cmd
        void pars_cmd(std::string buffer, Client &local_client);
        void join(Client &client, std::vector<std::string> &cmd);
        void topic(Client &client, std::string &cmd);
    // private:
        
};
std::vector<std::pair<std::string, std::string> > pars_join(std::vector<std::string> &cmd);

int string_to_int(const std::string &str);
char	**ft_split(char const *s, char c);
size_t table_size(char **table);


#endif