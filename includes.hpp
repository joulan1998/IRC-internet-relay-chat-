#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <stack>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#define PORT 9999
typedef struct s_client
{
    int fd;
    bool    authenticated;
}   t_client;

void    set_sockaddr(sockaddr_in **res, char *port);
void    bind_server(sockaddr_in *res, int server_fd);
void    listen_server(int server_fd);



#endif