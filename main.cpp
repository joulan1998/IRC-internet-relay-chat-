#include "includes.hpp"

int main(int argc , char **argv)
{
    if (argc != 3)
    {
        std::cout << "The number of arguments is not correct!" << std::endl;
        exit(1);
    }
    std::string password = argv[2];
    struct sockaddr_in *res;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(5);
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    set_sockaddr(&res, argv[1]);
    bind_server(res , server_fd);
    listen_server(server_fd);
    std::vector<pollfd> fds;
    std::vector<t_client> clients;
    clients.push_back((t_client){server_fd, true});
    fds.push_back((pollfd){server_fd, POLLIN, 0});
    char buffer[1024];
    while(true)
    {
        std::string well_msg;
        int client_fd;
        if (poll(fds.data(), fds.size(), 0) < 0)
        {
            perror("poll Error");
            exit(10);
        }
        size_t i;
        // for(i = 0; i < clients.size(); ++i)
        for(i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                sockaddr client_addr;
                socklen_t client_len = sizeof(client_addr);
                if (fds[i].fd == server_fd)
                {
                    well_msg = "WELCOME ! Please enter the password\n";
                    client_fd = accept(server_fd, &client_addr, &client_len);
                    if (send(client_fd, (well_msg.c_str()) , well_msg.size(), 0) < 0)
                    {
                        perror("send Error :");
                        exit(1);
                    }
                    fds.push_back((pollfd){client_fd, POLLIN, 0});
                    clients.push_back((t_client){client_fd, false});
                    std::cout << client_fd << ": " << buffer << std::endl;
                }
                else
                {
                    t_client &local_client = clients[i];
                    if (recv(client_fd, &buffer , 1023, 0) < 0)
                    {
                        perror("RECV Error :");
                        exit(1);
                    }
                    std::string msg(buffer);
                    if (!local_client.authenticated)
                    {
                        if (!strncmp((const char *)password.c_str(), msg.c_str(), msg.size()+1))
                        {
                            puts("authenticated ");
                            puts(msg.c_str());
                            local_client.authenticated = true;
                        }

                    }
                    // fds.push_back((pollfd){client_fd, POLLIN, 0});
                    t_client client ;
                    client.fd = fds[i].fd;

                    std::cout << client_fd << ": " << buffer << std::endl;
                }
            }
        }
        i = 0;
    }
    std::cout << "Server socket created.\n";
    close(server_fd);
    return 0;
}
