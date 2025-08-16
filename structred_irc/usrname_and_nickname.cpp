#include "inlcudes.hpp"

size_t table_size(char **table)
{
    size_t i = 0;
    while (table[i])
        i++;
    return (i);
}



void server::handle_nickname(client &local_client)
{
    char **table = NULL;
    char buffer[1024];
    bool result = false;

    while (!result)
    {
        memset(buffer, 0, sizeof(buffer));
        // free_table(table, table_size(table));      table freeing
        ssize_t bytes_received = recv(local_client.fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
        }
        else if (bytes_received == 0)
        {
            // here where i should handle when CTRL + C pressed !
                exit(11);
                // continue;
        }
        // Split the received buffer into tokens
        table = ft_split(buffer, ' ');
        if (*table == NULL)
            continue;
        if ((table_size(table) == 2))
        {
            table[0][4] = '\0'; //to replace '\n' by split<
            table[1][strlen(table[1])] = '\0'; //to replace '\n' by split<
            if ((!strncmp(table[0], "NICK\0", 5)))
            {
                puts("NICKNAME handling done!!");
                local_client.nickname = (table[1]);
                result = true;
                // return;
            }
        }
    }
    // should  Free allocated memory for table
}

void server::handle_username(client &local_client)
{
    char **table = NULL;
    char buffer[1024];
    bool result = false;

    // fcntl(local_client.fd, F_SETFL, O_NONBLOCK);
    while (!result)
    {
        memset(buffer, 0, sizeof(buffer));
        // free_table(table, table_size(table));      table freeing
        ssize_t bytes_received = recv(local_client.fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
        }
        else if (bytes_received == 0)
        {
            // here where i should handle when CTRL + C pressed !
                exit(11);
                // continue;
        }
        // Split the received buffer into tokens
        table = ft_split(buffer, ' ');
        if (*table == NULL)
            continue;
        if ((table_size(table) == 2))
        {
            table[0][4] = '\0'; //to replace '\n' by split<
            table[1][strlen(table[1])] = '\0'; //to replace '\n' by split<
            if ((!strncmp(table[0], "USER\0", 5)))
            {
                puts("USERNAME handling done!!");
                local_client.username = (table[1]);
                result = true;
                // return;
            }
        }
    }
    // should  Free allocated memory for table
}
