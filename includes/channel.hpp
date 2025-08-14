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
        std::string topic;
        int limit;
        bool k;
        bool l;
        // bool i;
    public:
        void setFlag_l(set flag_l);// TODO ana hbest nena
        std::string getFlag_l();
        void setFlag_k(bool setflag);
        bool getFlag_k();
        void setPassword(std::string passw);
        std::string getPassword();
        void setTopic(std::string settopic);
        std::string getTopic();
        Channel(const std::string &name);
        std::string getName_channel();
        void addoperator(Client* _client); 
        bool isoperator(Client* _client);
        void addclient(Client* _client);
        bool check_operator(Client* _client);
        int is_client(Client* _client);
        void send_msg_in_channel(std::string msg);
};

#endif