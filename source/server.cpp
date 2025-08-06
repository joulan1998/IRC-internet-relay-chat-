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
    this->_port = std::stoi(port);
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
    char *again_msg = (char *)"please renter the passswor !\r\n";
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
    }
    else
        send(local_client.fd, again_msg, strlen(again_msg), 0);
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
    set_non_clocking();
    set_socket_addr();
    bind_server();
    set_listen();


    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back((Client){this->_socket_fd, true});
    
    char buffer[1024];
    puts(this->_password.c_str());
// }
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
                std::string welcome_str = "please enter the new password \n";
                sockaddr client_addr;
                socklen_t client_len = sizeof(client_addr);
                if (this->fds[i].fd == this->_socket_fd)
                {
                    client_fd = accept(this->_socket_fd, &client_addr, &client_len);
                    if (send(client_fd, welcome_str.c_str(), welcome_str.size(), 0) < 0)
                        throw(std::runtime_error("send_error : " + std::string(strerror(errno))));
                    this->fds.push_back((pollfd){client_fd, POLLIN, 0});
                    // std::find(this->clients.begin(), this->fds->end(), this->clients.firs)     <<<<<<<<<<<<<<<<<<<<<<<<<
                    this->clients.push_back((Client){client_fd, false});
                }
                else
                {
                    Client &local_client = this->clients[i];
                    memset(buffer, 0, 1024);
                    if (recv(local_client.fd, &buffer, 1024, 0) < 0)
                        throw(std::runtime_error("recv_error : " + std::string(strerror(errno))));
                    if (!local_client.authenticated)
                    {
                        // puts("will get connection ");
                        handle_new_client(local_client, buffer, i);
                    }
                    else //(*buffer)
                    {
                        std::vector<std::string> split_buffer = split(buffer, ' ');
                        if (split_buffer.size() < 2)
                            std::cout << "invalid argiment" << std::endl;
                        if (split_buffer[0] == "join")
                            join(&local_client, split_buffer);
                        // for(size_t i = 0 ; i < split_buffer.size(); i++)
                        // {
                        //     std::cout << split_buffer[i] << std::endl;
                        // }
                        // handle_message(local_client, buffer, i);
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
        if (name_channel == channels[i].getName_channel())
            return &channels[i];
    }
    std::cout << "channel not found :: "<< name_channel<< std::endl;
    return NULL;
}


int Server::addchannel(const std::string &name_channel)
{

    std::cout<<"------------------>"<< name_channel<<std::endl;
    if (name_channel[0] == '#')
        channels.push_back(Channel(name_channel));
    else 
       return (std::cout << "channel not created :: "<< name_channel<< std::endl, 1);
    return 0;
}

std::map<std::string, std::string> pars_join(std::vector<std::string> &cmd)
{
    std::map<std::string, std::string> last_cmd;
    std::vector<std::string> keys;
    std::vector<std::string> channels;

    if (cmd.size() > 2)
    {
        keys = split(cmd[2].c_str(), ',');
        //for debagin
        // std::cout << "--------+++++++++++------------\n";
        // for (size_t i = 0; i < keys.size(); i++){std::cout <<"[" <<keys[i]<<"]"<<std::endl;}
    }
    
    channels = split(cmd[1].c_str(), ',');
    for(size_t j = 0; j < channels.size(); j++)
    {
        if (keys.size() > j)
            last_cmd[channels[j]] = keys[j];
        else
            last_cmd[channels[j]] = "";
    }

    // for (std::map<std::string, std::string>::iterator it = last_cmd.begin(); it != last_cmd.end(); ++it)
    //     std::cout << it->first << "  ==  " << it->second << std::endl;
    return last_cmd;

}

void Server::join(Client *client, std::vector<std::string> &cmd)
{
    std::map<std::string, std::string> channel_pass = pars_join(cmd) ;

    for (std::map<std::string, std::string>::iterator it = channel_pass.begin(); it != channel_pass.end(); ++it)
    {
        Channel *check_channel = getchannel(it->first);
        if(!check_channel)
        {
            if (addchannel(it->first) == 0)// creatchannel
            {
                check_channel = getchannel(it->first);
                check_channel->addoperator(client);
            }
            else
            {
                std::string err = ERR_NOSUCHCHANNEL(it->first);
                if (send(client->fd, err.c_str(), err.length(), 0) < 0)
                    std::cerr<< "msg not send"<<std::endl;
            }
            check_channel->addclient(client);
            std::string hh ="dsaf dfadf fasfd fsa fa  fasdf";
            check_channel->send_msg_in_channel(hh, client->fd);
        }
            // std::cout << it->first << "  ==  " << it->second << std::endl;
    }

	// 	// sendmsg(fd,)
    // }
}


