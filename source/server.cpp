#include  "../includes/server.hpp"
#include  "../includes/channel.hpp"
#include "../includes/includes.hpp"

void Server::create_socket()
{
    int local_socket;
    local_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (local_socket < 0 )
    {
        throw(std::runtime_error("socket_error" + std::string(strerror(errno))));
    }
    this->_socket_fd = local_socket;
}

void Server::set_non_clocking()
{
    fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK);
}

void    Server::socket_options()
{
    int option = 1;
    if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR ,&option , sizeof(option)) < 0)
    {
        perror("SET SOCKOPT ERROR : ");
        exit(1);
    }
}

void Server::set_socket_addr()
{
    this->_socket_addr = new sockaddr_in;
    if (!this->_socket_addr)
         throw(std::runtime_error("allocation_error : " + std::string(strerror(errno))));
    this->_socket_addr->sin_family = AF_INET;
    this->_socket_addr->sin_port = htons(this->_port);;
    this->_socket_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
}

Server::Server(std::string port, std::string password)
{
    this->_password = password;
    this->_port = string_to_int(port);
    // this->_port = std::stoi(port);
}

void Server::set_listen()
{
    if (listen(this->_socket_fd, MAX_CLIENT) < 0)
    {
        throw(std::runtime_error("listen_error : " + std::string(strerror(errno))));
    }
}

void Server::bind_server()
{
    if (bind(this->_socket_fd, (struct sockaddr *)(this->_socket_addr), sizeof(sockaddr)) < 0)
    {
        throw(std::runtime_error("bind_error : " + std::string(strerror(errno))));
    }
}


void Server::handle_new_client(Client &local_client, char *buffer, int i)
{
    char *again_msg = (char *)"please renter the password !\r\n";
    if (!*buffer)
    {
        close(local_client.fd);
        this->clients.erase(this->clients.begin() + i);
        this->fds.erase(this->fds.begin() + i);
    }
    else if (!strncmp(buffer, this->_password.c_str(), strlen(buffer + 1)))
    {
        std::cout << "NOW YOU ARE CONNECTED !!!" << std::endl;
        local_client.authenticated = true;

        handle_username(local_client/*, i*/);
        handle_nickname(local_client/*, i*/);
    }
    else
    {
        if (send(local_client.fd, again_msg, strlen(again_msg), 0) < 0)
            perror("send_ERROR :");
    }
}

void    Server::handle_message(Client &local_client, char *buffer, int index)
{
    if (!*buffer)
    {
        close(local_client.fd);
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
        // exit(8);
    }
    if (strncmp("exit\n", buffer, 5) == 0)
        exit(9);
    std::cout << "Client " << local_client.fd << " :" << " THE NICKNAME :" << local_client.nickname << " the username : " << local_client.username  << " THE MESSAGE :" << buffer;
}
int Server::handle_nickname(Client &local_client)
{
    char nick[1024] = {0};
    const char message[] = "PLEASE ENTER NICKNAME:\n";

    fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
    if (send(local_client.fd, message, strlen(message), 0) < 0)
    {
        perror("SEND ERROR");
        return -1;
    }
    ssize_t bytes_received = 0;
    while (true)
    {
        bytes_received = recv(local_client.fd, nick, sizeof(nick) - 1, 0);
        if (bytes_received > 0)
        {
            nick[bytes_received+1] = '\0'; // Null-terminate the string
            break;
        }
        else if (bytes_received == 0)
        {
            std::cerr << "Client disconnected while entering nickname." << std::endl;
            return -1;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            usleep(1000);
            continue;
        }
        else
        {
            perror("RECV ERROR");
            return -1;
        }
    }
    std::string n(nick);
    cleanString(n);     //ana lidert ahada bach nehayed \r \n
    local_client.nickname = strdup(n.c_str());
    std::cout << "SUCCESS! Nickname set to: " << local_client.nickname << std::endl;
    return 0;
}

int Server::handle_username(Client &local_client)
{
    char username[1024] = {0};
    const char message[] = "PLEASE ENTER USERNAME:\n";

    fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
    if (send(local_client.fd, message, strlen(message), 0) < 0)
    {
        perror("SEND ERROR");
        return -1; // Return error code
    }

    ssize_t bytes_received = 0;
    while (true)
    {
        bytes_received = recv(local_client.fd, username, sizeof(username) - 1, 0);

        if (bytes_received > 0)
        {
            
            username[bytes_received +1] = '\0';
            break;
        }
        else if (bytes_received == 0)
        {
            std::cerr << "Client disconnected while entering username." << std::endl;
            return -1;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            usleep(1000);
            continue;
        }
        else
        {
            perror("RECV ERROR");
            return -1;
        }
    }
    std::string user(username); 
    cleanString(user);//ana lidert ahada bach nehayed \r \n
    local_client.username = strdup(user.c_str());
    std::cout << "SUCCESS! Username set to: " << local_client.username << std::endl;
    return 0;
}

void Server::start_server()
{
    create_socket();
    set_non_clocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    // signal(SIGINT, test_fun(SIGINT));


    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back((Client){this->_socket_fd, true,"", ""});
    
    char buffer[1024];
    puts(this->_password.c_str());
    size_t i =0;
    
    while(1)
    {
        int client_fd;
        if (poll(fds.data(), fds.size(), -1) < 0)
            throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
        for (i = 0; i < fds.size(); i++)
        {
            if (this->fds[i].revents & POLLIN)
            {
                std::string welcome_str = "please enter the password \n";
                sockaddr client_addr;
                socklen_t client_len = sizeof(client_addr);
                if (this->fds[i].fd == this->_socket_fd)
                {
                    client_fd = accept(this->_socket_fd, &client_addr, &client_len);
                    if (send(client_fd, welcome_str.c_str(), welcome_str.size(), 0) < 0)
                        throw(std::runtime_error("send_error : " + std::string(strerror(errno))));
                    this->fds.push_back((pollfd){client_fd, POLLIN, 0});
                    // std::find(this->clients.begin(), this->fds->end(), this->clients.firs)     <<<<<<<<<<<<<<<<<<<<<<<<<
                    this->clients.push_back((Client){client_fd, false, "", ""});
                }
                else
                {
                    Client &local_client = this->clients[i];
                    // fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
                    memset(buffer, 0, 1024);
                    if (recv(local_client.fd, &buffer, 1024, 0) < 0)
                        throw(std::runtime_error("recv_error : " + std::string(strerror(errno))));
                    if (!local_client.authenticated)
                    {
                        handle_new_client(local_client, buffer, i);
                    }
                    else //(*buffer)
                    {
                        std::string new_buffer(buffer);
                        if (new_buffer.empty())
                            return;
                        std::vector<std::string> split_buffer = split(new_buffer, ' ', false);
                        if (split_buffer.size() && split_buffer[0] == "join")
                            join(&local_client, split_buffer);
                        else if (split_buffer.size() && split_buffer[0] == "topic")
                            topic(&local_client, buffer);
                        else if(split_buffer.size())
                            print_error(local_client.fd,ERR_UNKNOWNCOMMAND(split_buffer[0]) );

                        //  handle_message(local_client, buffer, i);
                    }
                }
            }
        }

    }

}
//////////////channel
Channel* Server::getchannel(const std::string &name_channel)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if (!name_channel.empty() && name_channel == channels[i].getName_channel())
            return &channels[i];
    }
    std::cout << "channel not found :: "<< name_channel<< std::endl;
    return NULL;
}


int Server::addchannel(Client *_client, const std::string &name_channel)
{

    std::cout<<"------------------>"<< name_channel<<std::endl;
    if (name_channel[0] == '#' && name_channel[1] != '\0')
        channels.push_back(Channel(name_channel));
    else
    {
        std::string msg = ERR_NOSUCHCHANNEL(name_channel);
        send(_client->fd, msg.c_str(), msg.length(), 0);
        return (1);
    }
    return 0;
}

std::vector<std::pair<std::string, std::string> > pars_join(std::vector<std::string> &cmd)
{
    std::vector<std::pair<std::string, std::string> > last_cmd;
    std::vector<std::string> keys;
    std::vector<std::string> channels;
    std::string password;
    if (cmd.size() > 2){keys = split(cmd[2], ',', true);}
    
    channels = split(cmd[1], ',', true);
    std::cout << "----------------------------------\n";

    for (size_t i = 0; i < channels.size(); i++)
    {
        std::cout << channels[i] << std::endl;
    }
    std::cout << "----------------------------------\n";
    for(size_t j = 0; j < channels.size(); j++)
    {
        password = "";
        if (keys.size() > j)
            password = keys[j];
        last_cmd.push_back(std::make_pair(channels[j], password));
    }
    for (size_t i = 0; last_cmd.size() > i; i++)
        std::cout << last_cmd[i].first << "  ==  " << last_cmd[i].second << std::endl;
    return last_cmd;

}

void Server::join(Client *client, std::vector<std::string> &cmd)
{
    if(cmd.size()<2)
    {
        print_error(client->fd, ERR_NEEDMOREPARAMS(client->username));
        return;
    }
    std::vector<std::pair<std::string, std::string> > ch_pass = pars_join(cmd) ;
    std::string msg;
    for (size_t i = 0; i < ch_pass.size(); i++)
    {
        Channel *ch = getchannel(ch_pass[i].first);
        if(!ch)
        {
            if (addchannel(client, ch_pass[i].first) == 0)
            {
                ch = getchannel(ch_pass[i].first);
                ch->addoperator(client);
                if (ch_pass[i].second.empty())//TODO hada tah ghir tmp bach n testi bih
                {
                    ch->setPassword(ch_pass[i].second);
                    ch->setFlag_k(true);
                }
                ch->setFlag_l(true);
                ch->setLimit(2);
            }
        }

        if (ch)
        {
            if (ch->getFlag_l() && (ch->getLimit() == (ch->getOperators().size() + ch->getClients().size())))
            {
                print_error(client->fd, ERR_CHANNELISFULL(client->nickname ,ch->getName_channel()));
                    continue;
            }
            if (ch->getFlag_k()  && ch->getPassword() != ch_pass[i].second)
            {
                print_error(client->fd, ERR_BADCHANNELKEY(ch->getName_channel()));
                continue;
            }
            if(!ch->is_client(client) )
            {
                if (!ch->check_operator(client))
                    ch->addclient(client);
                ch->send_msg_in_channel(RPL_JOIN(client->nickname,ch->getName_channel()));
                print_error(client->fd, RPL_NAMREPLY(client->nickname, ch->getName_channel(), ch->list_of_client()));
                print_error(client->fd, RPL_ENDOFNAMES(client->nickname, ch->getName_channel()));
            }
        }
    }
}




///////////////////////////////////////////////////////////////////////////

void Server::print_error(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not send "<< std::endl;
}

//TODO KHASNI NZID ILA CHANNEL KHAS GHIR ADMIN LI BDEL TOPIC
void Server::topic(Client *client,  std::string cmd)
{
    std::vector<std::string> new_cmd = split(cmd, ' ',false);
    if (new_cmd.size() == 1 && new_cmd[0] == "topic ")
    {
        print_error(client->fd, ERR_NEEDMOREPARAMS(client->nickname));
        return;
    }
    Channel *ch = getchannel(new_cmd[1]);
    if (!ch)
    {
        print_error(client->fd, ERR_NOSUCHCHANNEL(new_cmd[1]));
        return;
    }
    if (!ch->is_client(client) ) //TODO && check is operator
    {
        print_error(client->fd, ERR_NOTONCHANNEL(client->nickname, new_cmd[1]));
        return;
    }
    if (new_cmd.size() == 2)
    {
        if (ch->getTopic().empty())
        {
            print_error(client->fd, RPL_NOTOPIC(client->nickname, new_cmd[1]));
            return;
        }
        else 
        {
            print_error(client->fd, RPL_TOPIC(client->nickname, new_cmd[1], ch->getTopic()));
            std::cout << ch->getTopic();
            return;
        }
    } 
    if (new_cmd.size() > 2)
    {
        std::string ii = cmd;
        size_t index = ii.find(new_cmd[1]) + new_cmd[1].length();
        while(cmd[index] == ' ') index++;
        if (cmd[index] != ':')
            ch->setTopic(new_cmd[2] + POSTFIX);
        else if (cmd[index] == ':')
        {
            index++;
            ch->setTopic(&cmd[index]);
        }
        ii = ch->getTopic();
        ch->send_msg_in_channel(ii);

    }
}