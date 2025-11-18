#include "../includes/channel.hpp"
#include <algorithm>
Channel::Channel(const std::string &name) : name_channel(name),k(false),l(false),i(false),t(false){}

std::string Channel::getName_channel(){return name_channel;}
void Channel::setFlag_k(bool set_k){k = set_k;}
bool Channel::getFlag_k(){return k;}

std::vector<Client > &Channel::getInviteds(){return invited;}

bool Channel::getFlag_l(){return l;}
void Channel::setFlag_l(bool flag_l){l = flag_l;}

bool Channel::getFlag_i(){return i;}
void Channel::setFlag_i(bool flag_i){i = flag_i;}

bool Channel::getFlag_t(){return t;}
void Channel::setFlag_t(bool flag_t){t = flag_t;}


void Channel::setLimit(size_t l){limit = l;}
size_t Channel::getLimit(){return limit;}

std::string Channel::getTopic(){return topic;};
void Channel::setTopic(std::string settopic){topic = settopic;}

std::vector<Client > &Channel::getOperators(){return op;}
std::vector<Client > &Channel::getClients(){return clients;}

void Channel::setPassword(std::string pass){password = pass;}
std::string Channel::getPassword(){return password;}


void Channel::addoperator(Client &_client)
{
    bool check = false;
    
    for (size_t i = 0; i < op.size(); i++)
    {
        // if (op[i].fd == _client.fd)
        if (op[i].get_fd() == _client.get_fd())
        {
            check = true;
            break;
        }
    }
    if(!check)
        op.push_back(_client);
}

void Channel::add_invited(Client &_client)
{
    bool check = false;
    
    for (size_t i = 0; i < invited.size(); i++)
    {
        // if (invited[i].fd == _client.fd)
        if (invited[i].get_fd() == _client.get_fd())
        {
            check = true;
            break;
        }
    }
    if(!check)
        invited.push_back(_client);
    for(size_t i = 0; i > invited.size(); i++)
    {
        std::cout << invited[i].get_nickname()<<std::endl;
    }
}

void Channel::addclient(Client &_client)
{
    bool check = false;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].get_fd() == _client.get_fd())
        {
            check = true;
            break;
        }
    }
    if(!check)
        clients.push_back(_client);
}

bool Channel::is_operator(Client &_client)
{
    for (size_t i = 0; i < op.size(); i++)
    {
        // if (op[i].fd == _client.fd)
        if (op[i].get_fd() == _client.get_fd())
            return true;
    }
    return false;
}

bool Channel::is_client(Client &_client)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        // if(clients[i].fd == _client.fd)
        if(clients[i].get_fd() == _client.get_fd())
            return true;
    }
    return false;
}

bool Channel::is_invited(Client &_client)
{
    for (size_t i = 0; i < invited.size(); i++)
    {
        if(invited[i].get_fd() == _client.get_fd())
            return true;
    }
    return false;
}


void Channel::send_msg_in_channel(std::string msg)
{
    for(size_t i = 0; i < op.size(); i++)
    {
        if(send(op[i].get_fd(), msg.c_str(), msg.length(), 0) == -1)
            std::cerr<< "msg not send"<<std::endl;
    }
    for(size_t i = 0; i < clients.size(); i++)
    {
        if(send(clients[i].get_fd(), msg.c_str(), msg.length(), 0) == -1)
            std::cerr<< "msg not send"<<std::endl;
    }
}



std::string Channel::list_of_client()
{
    std::string last_string;

    for (size_t i = 0; i < op.size(); i++)
    {
        // last_string += '@' + op[i].nickname;
        last_string += '@' + op[i].get_nickname();
        if(i != op.size())
            last_string += " ";
    }
    for (size_t i = 0; i < clients.size(); i++)
    {
        // last_string += clients[i].nickname;
        last_string += clients[i].get_nickname();
        if(i != clients.size())
            last_string += " ";
    }
    return last_string;
}

// frenzy commands
void Channel::removeOperator(Client &_client) {
    for (size_t i = 0; i < op.size(); ++i) {
        if (op[i].get_fd() == _client.get_fd()) {
            op.erase(op.begin() + i);
            break;
        }
    }
}