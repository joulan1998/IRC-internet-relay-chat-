#include "../includes/includes.hpp"
Server *reff;

void sig_handler(int sig)
{
    (void) sig;
    size_t i = 0;
    while (i < reff->clients.size())
    {
        close(reff->clients[i].get_fd());
        i++;
    }
    close(reff->get_socket_fd());
    delete(reff->_socket_addr);
    exit(0);
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
                return;
            }
        }
        for (size_t i = 0; i < this->getOperators().size(); i++)
        {
            if (this->op[i].get_nickname() == nickname)
            {
                this->op.erase(this->op.begin() + i);
                return;
            }
        }
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
}

Server::Server(std::string port, std::string password)
{
    this->_password = password;
    this->_port = string_to_int(port);
}

void Server::set_listen()
{
    if (this->_socket_fd < 0)
        throw(std::runtime_error("Invalid socket file descriptor"));
    if (listen(this->_socket_fd, MAX_CLIENT) < 0)
        throw(std::runtime_error("listen_error: " + std::string(strerror(errno))));
}

void Server::bind_server()
{
    if (bind(this->_socket_fd, (struct sockaddr *)(this->_socket_addr), sizeof(sockaddr)) < 0)
        throw(std::runtime_error("bind_error : " + std::string(strerror(errno))));
}

void Server::print_msg(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not sent !"<< std::endl;
}

void Server::start_server()
{
    create_socket();
    set_non_blocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    reff = this;
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);
    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back(Client(this->_socket_fd));
    size_t i =0;
    ssize_t bytes_readen = 0;
    char buffer[1024];
    std::string new_buffer;
    std::cout << "-------SERVER START SUCCESSFULLY-------"<< std::endl;
    while(1)
    {
        int client_fd;
        poll(fds.data(), fds.size(), -1);
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
                }
                else
                {
                    Client &local_client = this->clients[i];
                    sockaddr_in* addr_in = (sockaddr_in*)&client_addr;
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
                    std::string client_ip(ip_str);
                    local_client.set_host(client_ip);
                    fcntl(local_client.get_fd(), F_SETFL, O_NONBLOCK);
                    memset(buffer, 0,1024);
                    bytes_readen = recv(local_client.get_fd(), buffer,1024, 0);

                    if (bytes_readen == 0 && clients.size() > 1 )
                    {
                        std::cout << "CLIENT <" << local_client.get_fd() << "> disconnected !" << std::endl;
                        this->clients.erase(this->clients.begin() + i);
                        ctrl_c_handling(local_client);
                        close((this->fds.begin() + i)->fd);
                        this->fds.erase(this->fds.begin() + i);
                        continue;
                    }
                    else if (bytes_readen < 0 )
                        throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
                    if (!local_client.get_authenticated() && local_client.get_fd() != this->_socket_fd)
                        handle_new_client(local_client, buffer, i);
                    else 
                    {
                        local_client.set_buffer_client(local_client.get_buffer_client() += buffer);
                        std::cout << "  the buffer before parsing : " << local_client.get_buffer_client() << std::endl;
                        if (local_client.get_buffer_client().find('\n') == std::string::npos)
                            continue;
                        pars_cmd(local_client.get_buffer_client(), local_client);
                        local_client.clear_buffer();
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
    else if (split_buffer.size() && split_buffer[0] == "INVITE")
        invite(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "QUIT")
        quit(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "KICK")
        kick(local_client , split_buffer, buffer);
    else if (split_buffer.size() && split_buffer[0] == "PONG 0");
    else
        print_error(local_client.get_fd(),ERR_UNKNOWNCOMMAND(split_buffer[0]));
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
