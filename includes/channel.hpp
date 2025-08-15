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
        std::vector<Client *> op;
        std::string password;
        std::string topic;
        //mode tmp hado ghir bach n deir bihom test f join mol l mode i9dar ihtajhom
        size_t limit;
        bool k;
        bool l;
        bool i;
    public:


        void setLimit(size_t l);
        size_t getLimit();
        void setFlag_k(bool setflag);
        bool getFlag_k();

        void setFlag_l(bool flag_l);
        bool getFlag_l();

        void setFlag_i(bool flag_i);
        bool getFlag_i();

        void setPassword(std::string passw);
        std::string getPassword();
        void setTopic(std::string settopic);
        std::string getTopic();
        Channel(const std::string &name);
        std::string getName_channel();
        void addoperator(Client* _client); 
        // bool isoperator(Client* _client);
        void addclient(Client* _client);
        bool check_operator(Client* _client);
        int is_client(Client* _client);
        void send_msg_in_channel(std::string msg);

        std::string list_of_client();

        std::vector<Client *> getOperators();
        std::vector<Client *> getClients();
};

#endif