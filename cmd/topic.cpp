#include "../includes/includes.hpp"

void Server::topic(Client &client,  std::string &cmd)
{
    std::vector<std::string> new_cmd = split(cmd, ' ',false);
    if(new_cmd.size() < 2)
    {
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(client.get_nickname()));
        return;
    }
    Channel *ch = getchannel(new_cmd[1]);
    if (!ch)
    {
        print_error(client.get_fd(), ERR_NOSUCHCHANNEL(new_cmd[1]));
        return;
    }
    if (!ch->is_client(client) && !ch->is_operator(client))
    {
        print_error(client.get_fd(), ERR_NOTONCHANNEL(client.get_nickname(), new_cmd[1]));
        return;
    }
    if (new_cmd.size() == 2)
    {
        if (ch->getTopic().empty())
        {
            print_error(client.get_fd(), RPL_NOTOPIC(client.get_nickname(), new_cmd[1]));
            return;
        }
        else 
        {
            print_error(client.get_fd(), RPL_TOPIC(client.get_nickname(), new_cmd[1], ch->getTopic()));
            std::cout << ch->getTopic();
            return;
        }
    }
    if (new_cmd.size() > 2)
    {
        if (ch->getFlag_t() && !(ch->is_operator(client)))
        {
            print_error(client.get_fd(), ERR_CHANOPRIVSNEEDED(new_cmd[1]));
            return;
        }
        size_t index = cmd.find(new_cmd[1]) + new_cmd[1].length();
        while(cmd[index] == ' ') index++;
        if (cmd[index] != ':')
            ch->setTopic(new_cmd[2] + POSTFIX);
        else if (cmd[index] == ':')
        {
            index++;
            ch->setTopic(&cmd[index]);
        }
        ch->send_msg_in_channel(RPL_TOPIC(client.get_nickname(), new_cmd[1], ch->getTopic()));
    }
}