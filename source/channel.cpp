#include "../includes/channel.hpp"
#include <algorithm>
Channel::Channel(const std::string &name) : name_channel(name){}

std::string Channel::getName_channel(){return name_channel;}

bool Channel::getFlag_l(){return l;}
void Channel::setFlag_l(bool flag_l){l = flag_l;}

void Channel::setLimit(size_t l){limit = l;}
size_t Channel::getLimit(){return limit;}

void Channel::setFlag_k(bool setflag){k = setflag;}
bool Channel::getFlag_k(){return k;}

std::vector<Client *> Channel::getOperators(){return op;}
std::vector<Client *> Channel::getClients(){return clients;}

void Channel::setPassword(std::string pass){password = pass;}
std::string Channel::getPassword(){return password;}

void Channel::addoperator(Client* _client)
{
    bool check = false;
    
    for (size_t i = 0; i < op.size(); i++)
    {
        if (op[i] == _client)
        {
            check = true;
            break;
        }
    }
    if(!check)
        op.push_back(_client);
}

bool Channel::check_operator(Client* _client)
{
     for (size_t i = 0; i < op.size(); i++)
    {
        if (op[i] == _client)
            return true;
    }
    return false;
}


void Channel::addclient(Client* _client)
{
    bool check = false;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i] == _client)
        {
            check = true;
            break;
        }
    }
    if(!check)
        clients.push_back(_client);
}
int Channel::is_client(Client* _client)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if(clients[i] == _client)
            return 1;
    }
    return 0;
}
void Channel::send_msg_in_channel(std::string msg)
{
    for(size_t i = 0; i < op.size(); i++)
    {
        if(send(op[i]->fd, msg.c_str(), msg.length(), 0) == -1)
            std::cerr<< "msg not send"<<std::endl;
    }
    for(size_t i = 0; i < clients.size(); i++)
    {
        send(clients[i]->fd, msg.c_str(), msg.length(), 0);
    }
}

std::string Channel::getTopic(){return topic;};
void Channel::setTopic(std::string settopic){topic = settopic;}


std::string Channel::list_of_client()
{
    std::string last_string;

    for (size_t i = 0; i < op.size(); i++)
    {
        last_string += '@' + op[i]->nickname;
        if(i != op.size())
            last_string += " ";
    }
    for (size_t i = 0; i < clients.size(); i++)
    {
        last_string += clients[i]->nickname;
        if(i != clients.size())
            last_string += " ";
    }
    return last_string;
}