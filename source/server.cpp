#include "../includes/includes.hpp"

Server* Server::instance = NULL;
Server *reff;
// the function for closing the fd
void test_fun(int sig)
{
    (void) sig;
    size_t i = 0;
    while (i > reff->clients.size())
    {
        close(reff->clients[i].get_fd());
        i++;
    }
    close(reff->_socket_fd);
    delete(reff->_socket_addr);
    return;
}
void Channel::remove_client(std::string nickname)
{
    if (!nickname.empty())
    {
        for (size_t i = 0; i < this->clients.size(); i++)
        {
            if (this->clients[i].get_nickname() == nickname)
            {
                this->clients.erase(this->clients.begin() + i);
                return; // Exit after removing the client
            }
        }
        for (size_t i = 0; i < this->getOperators().size(); i++)
        {
            if (this->op[i].get_nickname() == nickname)
            {
                this->op.erase(this->op.begin() + i);
                return; // Exit after removing the client
            }
        }
    }
}

void Server::handle_nickname(Client &local_client, std::vector<std::string> table)
{
    if (!local_client.get_nickname().empty())
    {
        print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
        return ;
    }
    if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return ;
    }
    else if (table.size() == 1)
    {
        std::string st= "NICK";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
    else if (table.size() != 2)
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_NONICKNAMEGIVEN(st));
        return;
    }
    else if ( !table[1].empty() && (check_nickname(table[1]) == 0))
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_NICKNAMEINUSE(st));
        return;
    }
    else if  (!table[1].empty())
    {
        local_client.set_nickname(table[1]);
        if (!local_client.get_nickname().empty() && !local_client.get_realname().empty() && !local_client.get_hostname().empty() && !local_client.get_servername().empty())
        {
            local_client.set_authenticated(true);
            //msg start for server after start
            print_error(local_client.get_fd(), RPL_WELCOME(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_YOURHOST(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_CREATED(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_MYINFO(local_client.get_nickname(), local_client.get_hostname()));
        }
        std::cout<<"Client <"<<local_client.get_fd()<<"> set NICK: "<< local_client.get_nickname() <<std::endl;
    }
    else 
        std::cout << "error nikname"<< std::endl;
}

void Server::handle_username(Client &local_client, std::vector<std::string> table)
{   
    if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return;
    }
    // else if (local_client.get_registred())
    else if (local_client.get_registred() && (table.size() == 5) )
    {
        local_client.set_username(table[1]);
        local_client.set_hostname(table[2]);
        local_client.set_servername(table[3]);
        local_client.set_realname(table[4]);
        std::cout << "Client <" << local_client.get_fd() << "> set USER: "<<local_client.get_username()<< " HOST: "<<local_client.get_hostname()<< " SERV: "<<local_client.get_servername()<< " REAL: "<<local_client.get_realname()<< std::endl;
        if(!local_client.get_nickname().empty())
        {
            local_client.set_authenticated(true);
            //msg start for server after start
            print_error(local_client.get_fd(), RPL_WELCOME(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_YOURHOST(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_CREATED(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_MYINFO(local_client.get_nickname(), local_client.get_hostname()));
        }

    }
    else 
    {
        std::string st= "USER";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
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
    this->_socket_addr->sin_addr.s_addr = inet_addr("0.0.0.0");
    // this->_socket_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
}

Server::Server(std::string port, std::string password)
{
    this->_password = password;
    this->_port = string_to_int(port);
    // this->_port = std::stoi(port);
}


// void Server::set_listen()
// {
//     if (listen(this->_socket_fd, MAX_CLIENT) < 0)
//     {
//         throw(std::runtime_error("listen_error : " + std::string(strerror(errno))));
//     }
// }
void Server::set_listen()
{
    if (this->_socket_fd < 0)
        throw(std::runtime_error("Invalid socket file descriptor"));

    if (listen(this->_socket_fd, MAX_CLIENT) < 0)
        throw(std::runtime_error("listen_error: " + std::string(strerror(errno))));

    std::cout << "Server listening on port " << this->_port << "..." << std::endl;
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
    //ddhjsdhjdhjhsdjsdssd
    if(value.empty())
    {
        std::string st= "PASS";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
    // else if (local_client.registred)
    else if (local_client.get_registred())
        std::cout << "you are already passed the password !" << std::endl;
    else if(!strncmp(this->_password.c_str(), value.c_str(), value.size() + 1))
    {
        // local_client.registred = true;
        local_client.set_registred(true);
        std::cout << "Client <" << local_client.get_fd() << "> authenticated successfully." << std::endl;
        // log_connection(local_client);
    }
    else
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH(st));
        return;
    }
}



// void Server::handle_new_client(Client &local_client)
void Server::handle_new_client(Client &local_client, std::string buffer, size_t index)
{
    std::string temp_buff(buffer);
    std::vector<std::string> table = split(temp_buff, ' ' ,false);
    if (table.empty())
        return;
    to_upper(table[0]);
    if(temp_buff.empty())
    {
        // close(local_client.fd);
        close(local_client.get_fd());
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
        return;
    }
    else if (((!strncmp(table[0].c_str(), "PASS\0", 5))) && !local_client.get_registred())
    {
        handle_password(local_client, table[1]);
    }
    else if (((!strncmp(table[0].c_str(), "NICK\0", 5))) )
    { 
        handle_nickname(local_client, table);
    }
    else if ((((!strncmp(table[0].c_str(), "USER\0", 5))) ) /*&& (table.size() == 5) && local_client.get_registred()*/)
    {
        handle_username(local_client, table);
    }
    else if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return ;
    }
    else //((!strncmp(table[0].c_str(), "USER\0", 5) || (!strncmp(table[0].c_str(), "NICK\0", 5)) )  && !local_client.get_registred()/* && (table.size() == 5)*/)
    {
        print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
    }

    // else if ((!strncmp(table[0].c_str(), "USER\0", 5) || (!strncmp(table[0].c_str(), "NICK\0", 5)) )  && !local_client.get_registred()/* && (table.size() == 5)*/)
    // {
    //     std::string st= "*";
    //     print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
    // }
    // else if (local_client.get_registred())
    // {
    //     print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
    // }
    // else
    // {
    //     std::string st= "*";
    //     print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
    // }

}

void Server::print_msg(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not send "<< std::endl;
}

void Server::start_server()
{
    create_socket();
    set_non_blocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    //    instance = this;
    reff = this;
    signal(SIGINT, test_fun);
    signal(SIGQUIT, test_fun);

    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back(Client(this->_socket_fd)); // <<<<<< should set REGISTRED to true

    size_t i =0;
    ssize_t bytes_readen = 0;
    char buffer[1024];
    std::cout << "-------SERVER START SUCCESSFULLY-------"<< std::endl;
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
                    std::cout << "Client: " << client_fd << " is connected.\r\n";
                    this->fds.push_back((pollfd){client_fd, POLLIN, 0});
                    this->clients.push_back(Client(client_fd));
                    // this->clients.push_back((Client){client_fd, false, false, "", "", "", "", ""});
                }
                else
                {
                    Client &local_client = this->clients[i];
                    //hadechi bach nejib host deyal wahed l user
                    sockaddr_in* addr_in = (sockaddr_in*)&client_addr;
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
                    std::string client_ip(ip_str);
                    local_client.set_host(client_ip);

                    fcntl(local_client.get_fd(), F_SETFL, O_NONBLOCK);
                    memset(buffer, 0,1024);
                    bytes_readen = recv(local_client.get_fd(), buffer,1024, 0);
                    if (bytes_readen == 0 && clients.size() > 1)
                    {
                        std::cout << "CLIENT <" << local_client.get_fd() << "> disconnected !" << std::endl ; 
                        this->clients.erase(this->clients.begin() + i);
                        close((this->fds.begin() + i)->fd);
                        this->fds.erase(this->fds.begin() + i);
                        continue;
                    }
                    else if (bytes_readen < 0 )
                        throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
                    if (!local_client.get_authenticated() && local_client.get_fd() != this->_socket_fd)
                    {
                        handle_new_client(local_client, buffer, i);
                    }
                    else 
                    {
                        std::string new_buffer(buffer);
                        pars_cmd(buffer, local_client);
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
    else if (split_buffer.size() && split_buffer[0] == "PRIVMSG")
        privmsg(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "MODE")
        mode(local_client, buffer);
    // else if (split_buffer.size() && split_buffer[0] == "INVITE")//
    //     invite(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "QUIT")
        quit(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "KICK")
        kick(local_client , split_buffer);                                          //<<<<<<<<<<<
    else if(split_buffer.size())
        print_error(local_client.get_fd(),ERR_UNKNOWNCOMMAND(split_buffer[0]) );
    
}

void Server::kick(Client &client, std::vector<std::string> table)
{
    // Ensure the command has the correct number of arguments
    std::string msg;
    if (table.size() != 3 && table.size() != 4) // KICK <channel> <user> [reason]
    {
        msg = "KICK";
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(msg));
        return;
    }
    std::string channel_name = table[1];
    std::string target_nickname = table[2];
    std::string reason = (table.size() > 3) ? table[3] : "No reason provided";
    // Check if the channel exists
    Channel* channel = getchannel(channel_name);
    if (!channel)
    {
        msg = channel_name;
        print_error(client.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
        return;
    }
    // Check if the target user exists
    Client* target_client = getClientByNick(target_nickname);
    if (!target_client)
    {
        msg = target_nickname;
        print_error(client.get_fd(), ERR_NOSUCHNICK(msg));
        return;
    }
    // Check if the target user is in the channel
    // if (!(channel->is_client(*(target_client))))
    // {

    //     msg = target_nickname;
    //     print_error(client.get_fd(), ERR_NOTONCHANNEL(channel->getName_channel(), target_client->get_nickname()));
    //     return;
    // }
    // Check if the client issuing the command has operator privileges
    if (!channel->is_operator(client))
    {
        print_error(client.get_fd(), ERR_CHANOPRIVSNEEDED(channel_name));
        return;
    }

    // Remove the target user from the channel
    channel->remove_client(target_client->get_nickname());
    channel->send_msg_in_channel(RPL_KICK(client.get_nickname(), target_client->get_nickname(), channel->getName_channel(), reason));
    if (channel->getOperators().empty() && channel->getClients().size())
    {
        channel->getOperators().push_back(channel->getClients()[0]);
        channel->getClients().erase(channel->getClients().begin());
        channel->send_msg_in_channel(RPL_UMODEIS(client.get_nickname(), channel->getName_channel(), "+o",channel->getOperators()[0].get_nickname()));
    }
}

int Server::check_nickname(std::string name)
{
    for (size_t i= 0 ;i < this->clients.size(); i++)
    {
        if (clients[i].get_nickname() == name)
        {
            return (0);
        }
    }
    return (1);
} 
