#include "inlcudes.hpp"

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
