#include "inlcudes.hpp"

size_t table_size(char **table)
{
    size_t i = 0;
    while (table[i])
        i++;
    return (i);
}

char   *server::extract_pass(char *username)
{
   size_t i = 4;
   size_t j = 0;
   char *buffer = NULL;
   if (strlen(username) >= 4)
        return(NULL);
   while(username[i])
   {
        if (std::isprint(username[i]))
            i++;
        else
        {
            buffer[j] = username[i];
            j++;
            i++;
        }
        if (!std::isprint(username[i]) && std::isprint(username[i-1]))
        {
            buffer[i]= '\0';
            break;
        }
    }
    return(buffer);
}
int server::handle_nickname(client &local_client)
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
        if (bytes_received == 0 || ((strlen(nick) == 1) && *nick == '\n'))
        {
            // std::cerr << "Client disconnected while entering nickname." << std::endl;
            return -1;
        }
        else if (bytes_received > 0)
        {
            nick[bytes_received+1] = '\0'; // Null-terminate the string
            local_client.nickname = strdup(nick);
            // continue;
            break;
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
    std::cout << "SUCCESS! Nickname set to: " << local_client.nickname << std::endl;
    return 0;
}

int server::handle_username(client &local_client)
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

        if (bytes_received == 0 || ((strlen(username) == 1) && *username == '\n'))
        {
            std::cerr << "Client disconnected while entering username." << std::endl;
            // handle_nickname()
            // continue;
            return -1;
        }
        else if (bytes_received > 0)
        {
            if ((username[0] == 'P') && (username[1] == 'A') && (username[0] == 'S') && (username[0] == 'S'))
            {
                std::cout << " extracted :: " << extract_pass(username) << std::endl; 
            }
                username[bytes_received +1] = '\0';
                local_client.username = strdup(username);
                // return 0;
                break;
            // }
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
    // local_client.username = strdup(username);
    std::cout << "SUCCESS! INT: " << bytes_received << std::endl;
    std::cout << "SUCCESS! Username set to: " << local_client.username << std::endl;
    return 0;
}
