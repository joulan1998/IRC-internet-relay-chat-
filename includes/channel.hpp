#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "includes.hpp"
#include "client.hpp"
#include <vector>
#include <iostream>
#include <sys/socket.h>

class Server;



class Channel
{
    private:
        std::string name_channel;
        std::vector<int> clients;
        std::vector<int > operators;
        std::string password;
        bool k;
        // bool l;
        // bool i;
    public:
        void setFlagk(bool setflag);
        bool getFlagk();
        void setPassword(std::string passw);
        std::string getPassword();
        Channel(std::string &name);
        std::string getName_channel();
        void addoperator(int fd); 
        bool isoperator(int fd);
        void addclient(int fd);
       void sendmsg (const std::string& msg, int fd);

};

#endif