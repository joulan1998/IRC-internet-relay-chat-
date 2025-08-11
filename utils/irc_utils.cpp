#include "../includes/includes.hpp"


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


int string_to_int(const std::string &str)
{
    std::istringstream iss(str);
    int value;

    // Try to read an int from the stream
    if (!(iss >> value))
    {
        throw std::invalid_argument("Invalid integer string: " + str);
    }

    return value;
}
//split cmd for channel
void cleanString(std::string &s)
{
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
}
std::vector<std::string> split(const char *str, char d)
{
    std::vector<std::string> res;
    std::string str1;
    for (std::size_t i = 0; i < std::strlen(str); ++i)
    {
        if((i==0 && str[i] == d) || (i == std::strlen(str) && str[i] == d))
        {
            str1 += "";
        }
        if (str[i] != d)
            str1 += str[i];
        else 
        {
            // if (!str1.empty())
            // {
                cleanString(str1);
                res.push_back(str1);
                str1.clear();
            // }
        }
    }
    if (!str1.empty())
    {
        cleanString(str1);
        res.push_back(str1);
    }
    return res;
}