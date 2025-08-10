#include "inlcudes.hpp"

void server::handle_new_client(client &local_client, char *buffer, int i)
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

void    server::handle_message(client &local_client, char *buffer, int index)
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


void server::start_server()
{
    create_socket();
    set_non_clocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    // signal(SIGINT, test_fun(SIGINT));


    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back((client){this->_socket_fd, true,"", ""});
    
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
                    this->clients.push_back((client){client_fd, false, "", ""});
                }
                else
                {
                    client &local_client = this->clients[i];
                    // fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
                    memset(buffer, 0, 1024);
                    if (recv(local_client.fd, &buffer, 1024, 0) < 0)
                        throw(std::runtime_error("recv_error : " + std::string(strerror(errno))));
                    if (!local_client.authenticated)
                    {
                        handle_new_client(local_client, buffer, i);
                    }
                    else
                        handle_message(local_client, buffer, i);
                }
            }
        }

    }

}
