#include "includes.hpp"


void    set_sockaddr(sockaddr_in **res, char *port)
{
    int local_port;
    local_port = atoi(port);
    std::cout << "the port numebr before " << local_port<< std::endl;
    (*res) = (sockaddr_in *)malloc(sizeof(sockaddr_in));
    if (!*res)
    {
        perror("Error allocations :");
        exit(1);
    };
    (*res)->sin_family = AF_INET;
    (*res)->sin_port = htons(local_port);
    (*res)->sin_addr.s_addr = inet_addr("127.0.0.1");
}

void bind_server(sockaddr_in *res, int server_fd)
{
    if (bind(server_fd, (struct sockaddr *)res, sizeof(sockaddr)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(6);
    }
}

void    listen_server(int server_fd)
{
    if (listen(server_fd, 10) == -1)
    {
        perror("listen");
        close(server_fd);
        exit(7);
    }
}