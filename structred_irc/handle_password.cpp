#include "inlcudes.hpp"

void server::handle_password(client &local_client)
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
            if ((!strncmp(table[0], "PASS\0", 5)) && (!strncmp(table[1], this->_password.c_str(), strlen(table[1]+1))))
            {
                puts("PASSOWRD handling done!!");
                result = true;
                // return;
            }
        }
    }
    // should  Free allocated memory for table
}