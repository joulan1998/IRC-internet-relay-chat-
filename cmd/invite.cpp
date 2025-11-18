#include "../includes/includes.hpp"

void Server::invite(Client &client, std::string &cmd)
{
    std::vector<std::string> new_cmd = split(cmd, ' ', false);
    if (new_cmd.size() != 3)
    {
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(client.get_nickname()));
        return;
    }
    Channel *ch = getchannel(new_cmd[2]);
    if (!ch)
    {
        print_error(client.get_fd(), ERR_NOSUCHCHANNEL(new_cmd[2]));
        return;
    }
    if (!ch->is_client(client) && !ch->is_operator(client))
    {
        print_error(client.get_fd(), ERR_NOTONCHANNEL(client.get_nickname(), new_cmd[2]));
        return;
    }
    if (!check_nickname(new_cmd[1]))
    {
        print_error(client.get_fd(), ERR_NOSUCHNICK(new_cmd[1]));
        return;
    }
    Client *target = getClientByNick(new_cmd[1]);
    if (ch->is_client(*target) || ch->is_operator(*target))
    {
        print_error(client.get_fd(), ERR_USERONCHANNEL(ch->getName_channel(), client.get_nickname()));
        return;
    }
    if (!ch->is_invited(*target))
        ch->add_invited(*target);
    print_msg(target->get_fd(), RPL_INVITE(client.get_nickname(),target->get_nickname(), new_cmd[2]));
    print_msg(client.get_fd(), RPL_INVITING(client.get_nickname(), target->get_nickname(), new_cmd[2]));
}