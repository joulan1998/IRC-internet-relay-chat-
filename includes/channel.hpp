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
        std::vector<Client *> clients;
        std::vector<Client *> operators;
        std::string password;
        bool k;
        // bool l;
        // bool i;
    public:
        void setFlagk(bool setflag);
        bool getFlagk();
        void setPassword(std::string passw);
        std::string getPassword();
        Channel(const std::string &name);
        std::string getName_channel();
        void addoperator(Client* _client); 
        bool isoperator(Client* _client);
        void addclient(Client* _client);
        void send_msg_in_channel(std::string& msg, int fd);

};

#endif