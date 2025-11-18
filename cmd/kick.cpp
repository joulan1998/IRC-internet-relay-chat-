#include "../includes/includes.hpp"

void Server::kick(Client &client, std::vector<std::string> &table , std::string &buffer)
{
    std::string msg;
if (table.size() < 3)
    {
        msg = "KICK";
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(msg));
        return;
    }
    std::string channel_name = table[1];
    std::string target_nickname = table[2];
    std::string reason ;
    if (table.size() > 3)
    {
        if (table[3][0] == ':' && table.size() > 4)
            reason = std::string(buffer, buffer.find(':') + 1 , buffer.size() - buffer.find(':'));
        else
            reason = std::string(table[3]);

    }
    // Check if the channel exists
    Channel* channel = getchannel(channel_name);
    if (!channel)
    {
        msg = channel_name;
        print_error(client.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
        return;
    }
    // Check if the target user exists
    Client* target_client = getClientByNick(target_nickname);
    if (!target_client)
    {
        msg = target_nickname;
        print_error(client.get_fd(), ERR_NOSUCHNICK(msg));
        return;
    }
    if (!(channel->is_client(*(target_client))))
    {

        msg = target_nickname;
        print_error(client.get_fd(), ERR_NOTONCHANNEL(channel->getName_channel(), target_client->get_nickname()));
        return;
    }
    if (!channel->is_operator(client))
    {
        print_error(client.get_fd(), ERR_CHANOPRIVSNEEDED(channel_name));
        return;
    }
    channel->remove_client(target_client->get_nickname());
    channel->send_msg_in_channel(RPL_KICK(client.get_nickname(), target_client->get_nickname(), channel->getName_channel(), reason));
    if (channel->getOperators().empty() && channel->getClients().size())
    {
        channel->getOperators().push_back(channel->getClients()[0]);
        channel->getClients().erase(channel->getClients().begin());
        channel->send_msg_in_channel(RPL_UMODEIS(client.get_nickname(), channel->getName_channel(), "+o",channel->getOperators()[0].get_nickname()));
    }
    else if (channel->getClients().empty() && channel->getOperators().empty())
        clean_channels(* channel);
}

void Server::clean_channels(Channel &channel)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].getName_channel() == channel.getName_channel())
        {
            this->channels.erase(this->channels.begin() + i);
            break;
        }
    }
}