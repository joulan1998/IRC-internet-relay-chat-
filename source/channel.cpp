#include "../includes/channel.hpp"
#include <algorithm>
Channel::Channel(const std::string &name) : name_channel(name){}

std::string Channel::getName_channel(){return name_channel;}

void Channel::setFlagk(bool setflag){k = setflag;}
bool Channel::getFlagk(){return k;}

void Channel::setPassword(std::string pass){password = pass;}
std::string Channel::getPassword(){return password;}

void Channel::addoperator(Client* _client)
{
    bool check = false;
    
    for (size_t i = 0; i < operators.size(); i++)
    {
        if (operators[i] == _client)
        {
            check = true;
            break;
        }
    }
    if(!check)
        operators.push_back(_client);
}

bool Channel::check_operator(Client* _client)
{
     for (size_t i = 0; i < operators.size(); i++)
    {
        if (operators[i] == _client)
            return true;
    }
    return false;
}
bool Channel::isoperator(Client* _client){
    bool check = false;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i] == _client)
        {
            check = true;
            break;
        }
    }
    return check;
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
void Channel::send_msg_in_channel(std::string& msg)
{
    for(size_t i = 0; i < clients.size(); i++)
        send(clients[i]->fd, msg.c_str(), msg.length(), 0);
}

std::string Channel::getTopic(){return topic;};
void Channel::setTopic(std::string settopic){topic = settopic;}