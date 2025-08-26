#include  "../includes/server.hpp"
#include  "../includes/channel.hpp"
#include "../includes/includes.hpp"


size_t table_size(char **table)
{
    size_t i = 0;
    while (table[i])
        i++;
    return (i);
}



void Server::handle_nickname(Client &local_client, std::string value)
{
    if (!local_client.registred)
        std::cout << "please enter password in first !"<< std::endl;
    else if (value.empty())
        return;
    else if  (!value.empty())
        local_client.nickname = value;
    else 
        std::cout << "error nikname"<< std::endl;
}

void Server::handle_username(Client &local_client, std::string value)
{
    if (value.size() >= 4 && local_client.registred)
    {

        local_client.username = value;
        local_client.authenticated = true;
    }
    else 
        std::cout << "error username"<< std::endl;
}


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

void Server::set_non_blocking()
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

void Server::handle_password(Client &local_client, std::string value/*,size_t index*/)
{
    if(value.empty())
        return;
    else if (local_client.registred)
        std::cout << "you are already passed the password !" << std::endl;
    else if(!strncmp(this->_password.c_str(), value.c_str(), value.size()))
    {
        local_client.registred = true;
        // log_connection(local_client);
    }
    else 
        std::cout << "handle password erroor  !" << std::endl;
}



// void Server::handle_new_client(Client &local_client)
void Server::handle_new_client(Client &local_client, std::string buffer, size_t index)
{
    std::string temp_buff(buffer);
    std::vector<std::string> table = split(temp_buff, ' ' ,false);
    if (table.empty())
        return;
    if(temp_buff.empty())
    {
        exit(94);
        close(local_client.fd);
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
        return;
    }
    else if ((!strncmp(table[0].c_str(), "PASS\0", 5)) || (!strncmp(table[0].c_str(), "pass\0", 5)))
    {
        handle_password(local_client, table[1]/*, index*/);
    }
    else if ((!strncmp(table[0].c_str(), "NICK\0", 5)) || (!strncmp(table[0].c_str(), "nick\0", 5)))
        handle_nickname(local_client, table[1]);
    else if ((!strncmp(table[0].c_str(), "USER\0", 5)) || (!strncmp(table[0].c_str(), "user\0", 5)))
    {
        handle_username(local_client, table[1]);
        log_connection(local_client);        
        

    }
    else
        puts("unkown command during auth !");
}



void    Server::handle_message(Client &local_client, char *buffer, int index)
{
    std::cout << local_client.fd<<std::endl;
    std::cout << (int)*buffer << "  :" << buffer;
    if (!*buffer)
    {
        close(local_client.fd);
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
    }
    std::cout << "Client " << local_client.fd << " :" << buffer;

}


void Server::start_server()
{
    create_socket();
    set_non_blocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    // signal(SIGINT, test_fun(SIGINT));


    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back((Client){this->_socket_fd,false, true,"", ""});
    
    puts(this->_password.c_str());
    size_t i =0;
    char buffer[1024];
    while(1)
    {
        int client_fd;
        if (poll(fds.data(), fds.size(), -1) < 0)
            throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
        for (i = 0; i < fds.size(); i++)
        {
            if (this->fds[i].revents & POLLIN)
            {
                sockaddr client_addr;
                socklen_t client_len = sizeof(client_addr);
                if (this->fds[i].fd == this->_socket_fd)
                {
                    client_fd = accept(this->_socket_fd, &client_addr, &client_len);
                    this->fds.push_back((pollfd){client_fd, POLLIN, 0});
                    this->clients.push_back((Client){client_fd, false, false, "", ""});
                }
                else
                {
                    Client &local_client = this->clients[i];
                    fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
                        memset(buffer, 0,1024);
                        if (recv(local_client.fd, buffer,1024, 0) < 0 )
                            throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
                    if (!local_client.authenticated && local_client.fd != this->_socket_fd)
                        handle_new_client(local_client, buffer, i);
                    // if ((this->fds[i].revents & POLLIN) && (local_client.fd != this->_socket_fd) && (local_client.authenticated) && (local_client.fd != this->_socket_fd))
                    else 
                    {
                        std::string new_buffer(buffer);
                            //new function
                        std::vector<std::string> split_buffer = split(new_buffer, ' ', false);
                        if (split_buffer.size() && (split_buffer[0] == "join" || split_buffer[0] == "JOIN"))
                            join(local_client, split_buffer);
                        else if (split_buffer.size() && (split_buffer[0] == "topic" || split_buffer[0] == "TOPIC"))
                            topic(local_client, split_buffer[1]);
                        else if(split_buffer.size())
                            print_error(local_client.fd,ERR_UNKNOWNCOMMAND(split_buffer[0]) );

                        // memset(buffer, 0,1024);                  
                        // std::string new_buffer(buffer);
                        // new_buffer = "heuiiii";
                        // std::cout<< new_buffer<<std::endl;
                        // recv(local_client.fd, buffer, 1024, 0);
                        // pars_cmd(buffer, local_client);
                    }
                }
            }
        }

    }

}

void Server::pars_cmd(std::string buffer, Client &local_client)
{
    std::vector<std::string> split_buffer = split(buffer, ' ', false);
    if (!split_buffer.size()) return;
    to_upper(split_buffer[0]);
    if (split_buffer.size() && split_buffer[0] == "JOIN")
        join(local_client, split_buffer);
    else if (split_buffer.size() && split_buffer[0] == "TOPIC")
        topic(local_client, buffer);
    else if(split_buffer.size())
        print_error(local_client.fd,ERR_UNKNOWNCOMMAND(split_buffer[0]) );
}


//////////////channel
Channel* Server::getchannel(const std::string &name_channel)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if (!name_channel.empty() && name_channel == channels[i].getName_channel())
            return &channels[i];
    }
    return NULL;
}


int Server::addchannel(Client _client, const std::string &name_channel)
{

    if (name_channel[0] == '#' && name_channel[1] != '\0')
        channels.push_back(Channel(name_channel));
    else
    {
        print_error(_client.fd, ERR_NOSUCHCHANNEL(name_channel));
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
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::cout << channels[i] << std::endl;
    }
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

void Server::join(Client &client, std::vector<std::string> &cmd)
{
    if(cmd.size() < 2 )
    {
        print_error(client.fd, ERR_NEEDMOREPARAMS(client.username));
        return;
    }
    std::vector<std::pair<std::string, std::string> > ch_pass = pars_join(cmd) ;
    std::string msg;
    for (size_t i = 0; i < ch_pass.size(); i++)
    {
        Channel *ch = getchannel(ch_pass[i].first);
        if (ch && (ch->is_client(client) || ch->is_operator(client)))
        {
            print_error(client.fd, ERR_USERONCHANNEL(ch->getName_channel(), client.nickname));
            continue;
        }
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
            if (ch->getFlag_i() && !(ch->is_invited(client)))
            {
                print_error(client.fd, ERR_INVITEONLYCHAN(client.nickname, ch->getName_channel()));
                continue;
            }
            if (ch->getFlag_l() && (ch->getLimit() == (ch->getOperators().size() + ch->getClients().size())))
            {
                print_error(client.fd, ERR_CHANNELISFULL(client.nickname ,ch->getName_channel()));
                    continue;
            }
            if (ch->getFlag_k()  && ch->getPassword() != ch_pass[i].second)
            {
                print_error(client.fd, ERR_BADCHANNELKEY(ch->getName_channel()));
                continue;
            }
            if(!ch->is_client(client) )
            {
                if (!ch->is_operator(client))
                    ch->addclient(client);

                ch->send_msg_in_channel(RPL_JOIN(client.nickname,ch->getName_channel()));
                print_error(client.fd, RPL_NAMREPLY(client.nickname, ch->getName_channel(), ch->list_of_client()));
                print_error(client.fd, RPL_ENDOFNAMES(client.nickname, ch->getName_channel()));

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
void Server::topic(Client &client,  std::string &cmd)
{
    std::vector<std::string> new_cmd = split(cmd, ' ',false);
    if(new_cmd.size() < 2)
    {
        print_error(client.fd, ERR_NEEDMOREPARAMS(client.nickname));
        return;
    }
 
    Channel *ch = getchannel(new_cmd[1]);
    if (!ch)
    {
        print_error(client.fd, ERR_NOSUCHCHANNEL(new_cmd[1]));
        return;
    }

    if (!ch->is_client(client) && !ch->is_operator(client))
    {
        print_error(client.fd, ERR_NOTONCHANNEL(client.nickname, new_cmd[1]));
        return;
    }
    if (new_cmd.size() == 2)
    {
        if (ch->getTopic().empty())
        {
            print_error(client.fd, RPL_NOTOPIC(client.nickname, new_cmd[1]));
            return;
        }
        else 
        {
            print_error(client.fd, RPL_TOPIC(client.nickname, new_cmd[1], ch->getTopic()));
            std::cout << ch->getTopic();
            return;
        }
    }

    if (new_cmd.size() > 2)
    {
            //new
        if (ch->getFlag_t() && !(ch->is_operator(client)))
        {
            print_error(client.fd, ERR_CHANOPRIVSNEEDED(new_cmd[1]));
            return;
        }
        size_t index = cmd.find(new_cmd[1]) + new_cmd[1].length();
        while(cmd[index] == ' ') index++;
        if (cmd[index] != ':')
            ch->setTopic(new_cmd[2] + POSTFIX);
        else if (cmd[index] == ':')
        {
            index++;
            ch->setTopic(&cmd[index]);
        }
        ch->send_msg_in_channel(RPL_TOPIC(client.nickname, new_cmd[1], ch->getTopic()));
    }
}