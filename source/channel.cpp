#include "../includes/channel.hpp"
#include <algorithm>
Channel::Channel(std::string &name) : name_channel(name){}

std::string Channel::getName_channel(){return name_channel;}

void Channel::setFlagk(bool setflag){k = setflag;}
bool Channel::getFlagk(){return k;}

void Channel::setPassword(std::string pass){password = pass;}
std::string Channel::getPassword(){return password;}

void Channel::addoperator(int fd)
{
    bool check = false;
    
    for (size_t i = 0; i < operators.size(); i++)
    {
        if (operators[i] == fd)
        {
            check = true;
            break;
        }
    }
    if(!check)
        operators.push_back(fd);
}

bool Channel::isoperator(int fd){
    bool check = false;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i] == fd)
        {
            check = true;
            break;
        }
    }
    return check;
}

void Channel::addclient(int fd)
{
    bool check = false;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i] == fd)
        {
            check = true;
            break;
        }
    }
    if(!check)
        clients.push_back(fd);

}
void Channel::sendmsg (const std::string& msg, int fd)
{
    for (size_t i = 0 ; i < clients.size(); i++) {
        if (clients[i] != fd) {
            send(clients[i],msg.c_str(), msg.size(), 0);
        }
    }
}