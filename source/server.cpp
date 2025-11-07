#include  "../includes/server.hpp"
#include  "../includes/channel.hpp"
#include "../includes/includes.hpp"

Server* Server::instance = NULL;
Server *reff;
// the function for closing the fd
void test_fun(int sig)
{
    (void) sig;
    size_t i =0;
    while (i > reff->clients.size())
    {
        close(reff->clients[i].get_fd());
        i++;
    }
    close(reff->_socket_fd);
    
    std::cout << reff->_socket_fd << std::endl;
    return;
}
void Server::handle_nickname(Client &local_client, std::string value)
{
    // if (!local_client.registred)
    if ( !value.empty() && (check_nickname(value) == 0))
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_NICKNAMEINUSE(st));
        return;
    }
    if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH2(st));
    }
        // std::cout << "please enter password in first !"<< std::endl;
    else if (value.empty())
    {
        std::string st= "NICK";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
    else if  (!value.empty())
    {
        local_client.set_nickname(value);
        std::cout<<"Client <"<<local_client.get_fd()<<"> set NICK: admin" << std::endl;
        // local_client.nickname = value;
    }
    else 
        std::cout << "error nikname"<< std::endl;
}

void Server::handle_username(Client &local_client, std::vector<std::string> table)
{   
    if ((table.size() != 5))
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH2(st));
        // std::cout << "error : use <USER username hostname servername realname> !"<< std::endl;
    }
    // else if (table[1].size() >= 4 && local_client.registred)
    else if (table[1].size() >= 4 && local_client.get_registred())
    {
        local_client.set_username(table[1]);
        local_client.set_hostname(table[2]);
        local_client.set_servername(table[3]);
        local_client.set_realname(table[4]);
        local_client.set_authenticated(true);
        print_error(local_client.get_fd(), RPL_WELCOME(local_client.get_nickname(), local_client.get_hostname()));
        print_error(local_client.get_fd(), RPL_YOURHOST(local_client.get_nickname(), local_client.get_hostname()));
        print_error(local_client.get_fd(), RPL_CREATED(local_client.get_nickname(), local_client.get_hostname()));
        print_error(local_client.get_fd(), RPL_MYINFO(local_client.get_nickname(), local_client.get_hostname()));
        // local_client.username = table[1];
        // local_client.hostname = table[2];
        // local_client.servername = table[3];
        // local_client.realname = table[4];
        // local_client.authenticated = true;
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
    else if(!strncmp(this->_password.c_str(), value.c_str(), value.size()))
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
    else if (((!strncmp(table[0].c_str(), "PASS\0", 5))) /*&& !local_client.get_registred()*/)
    {
        handle_password(local_client, table[1]/*, index*/);
    }
    else if (((!strncmp(table[0].c_str(), "NICK\0", 5))) )
        handle_nickname(local_client, table[1]);
    else if ((((!strncmp(table[0].c_str(), "USER\0", 5))) ) && /*(table.size() == 5) &&*/ local_client.get_registred())
    {
        handle_username(local_client, table);
    }
    else if ((((!strncmp(table[0].c_str(), "USER\0", 5 || (!strncmp(table[0].c_str(), "NICK\0", 5)) ) ) && !local_client.get_registred()/* && (table.size() == 5)*/)
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH2(st));
    }
    else if (local_client.get_registred())
    {
        print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
    }
    else
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH2(st));
    }

}

void Server::print_msg(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not send "<< std::endl;
}

void    Server::handle_message(Client &local_client, char *buffer, int index)
{
    // std::cout << local_client.fd<<std::endl;
    std::cout << local_client.get_fd()<<std::endl;
    std::cout << (int)*buffer << "  :" << buffer;
    if (!*buffer)
    {
        // close(local_client.fd);
        close(local_client.get_fd());
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
    }
    std::cout << "Client " << local_client.get_fd() << " :" << buffer;
    // std::cout << "Client " << local_client.fd << " :" << buffer;

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
                    //hadechi bach nejib host deyal wahed l user ///// /<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<------------------------------__________----------------------
                    sockaddr_in* addr_in = (sockaddr_in*)&client_addr;
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
                    std::string client_ip(ip_str);
                    local_client.set_host(client_ip);
                    // std::cout<< "--------- >>>>>> {"<<client_ip<<"} <<<<<< ----------"<< std::endl<<std::endl;
                    fcntl(local_client.get_fd(), F_SETFL, O_NONBLOCK);
                    memset(buffer, 0,1024);
                    bytes_readen = recv(local_client.get_fd(), buffer,1024, 0);
                    if (bytes_readen == 0 && clients.size() > 1)
                    {
                        std::cout << clients.size()<< std::endl;
                        this->clients.erase(this->clients.begin() + i);
                        close((this->fds.begin() + i)->fd);

                        // close(this->fds[this->fds.begin() + i].fd);
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
    else if (split_buffer.size() && split_buffer[0] == "QUIT")
        quit(local_client, buffer);
    else if(split_buffer.size())
        print_error(local_client.get_fd(),ERR_UNKNOWNCOMMAND(split_buffer[0]) );
    
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