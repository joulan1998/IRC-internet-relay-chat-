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

void Channel::send_msg_in_channel(std::string& msg, int fd)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(fd != clients[i]->fd)
            send(clients[i]->fd, msg.c_str(), msg.length(), 0);
    }
}