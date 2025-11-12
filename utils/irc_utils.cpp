#include "../includes/includes.hpp"

// void my_sleep(unsigned int seconds)
// {
//     usleep(seconds * 1000000); // 1 second = 1,000,000 microseconds
// }

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

std::vector<std::string> split(std::string &str, char delem, bool flag)
{
    std::vector<std::string> last_cmd;
    std::istringstream iss(str);
    std::string cmd;
    std::string new_str;

    if(flag == false)
    {
        while(iss >> cmd)
        {
            cleanString(cmd);
            last_cmd.push_back(cmd);
        }
    }
    else
    {
        size_t start = 0;
        size_t pos = str.find(delem);
        while (pos != std::string::npos)
        {
            new_str = str.substr(start, pos - start);
            // cleanString(new_str);
            last_cmd.push_back(new_str);
            start = pos+ 1;
            pos = str.find(delem, start);
        }
        last_cmd.push_back(str.substr(start));
    }
    return last_cmd;
}







void to_upper(std::string &str)
{
    std::string string;
    for (size_t i = 0; i < str.length(); i++)
    {    
        char c = toupper(str[i]);
        string += c;
    }   
    str = string;
}
