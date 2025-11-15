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
#include <signal.h>
class Channel;
class Client;

class Server
{
    private:
        std::vector<Channel> channels;
        std::vector<Channel> get_channels();
        static Server* instance;
        std::string _password;
        std::vector<pollfd> fds;
        int  _port;
        int  _socket_fd;
    public:
        struct sockaddr_in *_socket_addr;
        std::vector<Client> clients;
        Server(std::string port, std::string password);
        int     get_socket_fd(void);
        void    start_server();
        void    create_socket();
        void    set_non_blocking();
        void    set_socket_addr();
        void    socket_options();
        void    bind_server();
        void    set_listen();
        void    handle_new_client(Client &local_client, std::string value, size_t index);
        void    handle_password(Client &local_client, std::string value/*, size_t index*/);
        void    handle_message(Client &local_client, char *buffer, int i);
        void    handle_message(Client &local_client, size_t index);
        void    handle_username(Client &local_client, std::vector<std::string> table);
        void    handle_nickname(Client &local_client, std::vector<std::string> table);
        static void free_data(int sig);
        void print_error(int fd, std::string msg);
        void print_msg(int fd, std::string msg);
        void pars_cmd(std::string buffer, Client &local_client);
        void join(Client &client, std::vector<std::string> &cmd);
        void topic(Client &client, std::string &cmd);
        void quit(Client &client, std::string &cmd);
        void quit_handler(std::vector<Client> &cl, Client &client, std::vector<std::string> &new_cmd, size_t &i, std::string &cmd);
        void privmsg(Client &client, std::string &cmd);
        Client* getClientByNick(const std::string &nickname);
        void mode(Client &client, std::string &cmd);
        void invite(Client &client, std::string &cmd);
        int check_nickname(std::string name);
        void kick(Client &client, std::vector<std::string> table, std::string buffer);
        void clean_channels(Channel *channel);
        // channel commands
        Channel* getchannel(const std::string &name_channel);
        int addchannel(Client _client, const std::string &name_channel);
};
std::vector<std::pair<std::string, std::string> > pars_join(std::vector<std::string> &cmd);
int string_to_int(const std::string &str);


#endif