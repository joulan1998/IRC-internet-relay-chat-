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

    local_client.nickname = strdup(nick);
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
    local_client.username = strdup(username);
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
                        std::vector<std::string> split_buffer = split(new_buffer, ' ', false);
                        if (split_buffer.size() < 2)
                            std::cout << "invalid argiment" << std::endl;
                        if (split_buffer[0] == "join")
                            join(&local_client, split_buffer);
                        if (split_buffer[0] == "topic")
                            topic(&local_client, buffer);
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

std::map<std::string, std::string> pars_join(std::vector<std::string> &cmd)
{
    std::map<std::string, std::string> last_cmd;
    std::vector<std::string> keys;
    std::vector<std::string> channels;

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
        if (keys.size() > j)
            last_cmd[channels[j]] = keys[j];
        else
            last_cmd[channels[j]] = "";
    }
    for (std::map<std::string, std::string>::iterator it = last_cmd.begin(); it != last_cmd.end(); ++it)
        std::cout << it->first << "  ==  " << it->second << std::endl;
    return last_cmd;

}

void Server::join(Client *client, std::vector<std::string> &cmd)
{
    // for (size_t i = 0; i < cmd.size(); i++)
    // {
    //     std::cout<< cmd[i]<<std::endl;
    // }
    std::map<std::string, std::string> channel_pass = pars_join(cmd) ;
    std::string msg;
    for (std::map<std::string, std::string>::iterator it = channel_pass.begin(); it != channel_pass.end(); ++it)
    {
        Channel *check_channel = getchannel(it->first);
        if(!check_channel)
        {
            if (addchannel(client, it->first) == 0)
            {
                check_channel = getchannel(it->first);
                check_channel->addoperator(client);
                if (!(it->second.empty()))
                {
                    std::cout<< "erroooooooooooor\n";
                    check_channel->setPassword(it->second);
                    check_channel->setFlagk(true);
                }
            }
        }
        if (check_channel)
        {
            if(check_channel->getFlagk() == true)
            {
                if (check_channel->getPassword() != it->second)
                {
                    msg = ERR_BADCHANNELKEY(check_channel->getName_channel());
                    send(client->fd, msg.c_str(), msg.length(), 0);
                    continue;
                }
            }
            std::string ll= "hana henaaaaa\n";
            send(client->fd, ll.c_str(), ll.length(), 0);
            if(!check_channel->is_client(client))
            {
                check_channel->addclient(client);
                msg = RPL_JOIN("userrr_",check_channel->getName_channel());
                check_channel->send_msg_in_channel(msg);
                /*std::string msg1 = RPL_NAMREPLY("user_", check_channel->getName_channel(), "user");
                send(client->fd, msg1.c_str(), msg1.length(), 0);
                std::string msg2 = RPL_ENDOFNAMES("user_", check_channel->getName_channel());
                send(client->fd, msg2.c_str(), msg2.length(), 0);*/
            }
        }
    }
}




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


void Server::topic(Client *client,  std::string cmd)
{
    (void)client;
    std::vector<std::string> new_cmd = split(cmd, ' ',false);
    if (new_cmd.size() == 1 && new_cmd[0] == "topic")
    {
        std::string kk = "erooooor";
        send(client->fd, kk.c_str(), kk.length(), 0);
        return;
    }
    if (new_cmd.size() < 2)
    {
        std::cout << "errooooor \n";
        return;
    }
    Channel *ch = getchannel(new_cmd[1]);
    if (new_cmd.size() == 2)
    {
        if (!ch)
            std::cout << "makinach had channel";
        else {
            if (ch->getTopic().empty())
            {
                std::cout << "ma3andouch topic\n";
                return;
            }
            else 
            {
                std::cout << ch->getTopic();
                return;
            }
        }
    } 
    if (new_cmd.size() > 2)
    {
        std::string ii = cmd;
        size_t index = ii.find(new_cmd[1]);
        std::cout << index<<std::endl;
    }
    //ila makanch f cahnnel chi topic oja odar topic #ch :sdfsdfasd ghadi isift l ga3 l users b anaho dar topic;

}