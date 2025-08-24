#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>


class Client
{
    public:
        int fd;
        bool authenticated;
        bool registred;
        std::string nickname;
        std::string username;
};


#endif