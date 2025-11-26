#include "../includes/includes.hpp"


Client* Server::getClientByNick(const std::string &nickname) {
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].get_nickname() == nickname)
            return &clients[i];
    }
    return NULL;
}


void Server::mode(Client &client, std::string &cmd)
{
    std::vector<std::string> params = split(cmd, ' ', false);
    if (params.size() < 2)
    {
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(params[0]));
        return ;
    }
    Channel *channel = getchannel(params[1]);
    if (!channel)
    {
        print_error(client.get_fd(), ERR_NOSUCHCHANNEL(params[1]));
        return ;
    }
    if (params.size() == 2)
    {
        print_error(client.get_fd(), ERR_NOMODE(client.get_nickname(),params[1]));
        return ; 
    }
    if (!channel->is_operator(client))
    {
        print_error(client.get_fd(), ERR_CHANOPRIVSNEEDED(params[1]));
        return ;
    }
    if (params[2] == "+" || params[2] == "-")
    {
        print_error(client.get_fd(), ERR_UNKNOWNMODE(client.get_nickname(), params[1], params[0]));
        return;
    }
    char sign = '+';
    std::string appliedModes = "";
    std::string appliedParams = "";
    for (size_t k = 0; k < params[2].length(); ++k)
    {
        char c  = params[2][k];
        if (c == '+' || c == '-')
        {
            sign = c;
            continue;
        }
        bool needsParam =  (sign == '+' && (c == 'k' || c == 'l' || c == 'o')) || (sign == '-' && c == 'o');
        if (needsParam && (params.size() <= 3))
        {
            print_error(client.get_fd(), ERR_NEEDMOREPARAMS(params[0]));
            return ;
        }
        bool valid = true;
        switch(c) {
            case 'i':
                channel->setFlag_i(sign == '+');
                break;
            case 't':
                channel->setFlag_t(sign == '+');
                break;
            case 'k':
                if (sign == '+')
                {
                    channel->setPassword(params[3]);
                    channel->setFlag_k(true);
                }
                else
                {
                    channel->setPassword("");
                    channel->setFlag_k(false);
                }
                break;
            case 'l':
                if (sign == '+') 
                {
                    channel->setLimit(string_to_int(params[3]));
                    channel->setFlag_l(true);
                }
                else 
                {
                    channel->setLimit(0);
                    channel->setFlag_l(false);
                }
                break;
            case 'o': {
                Client *target = getClientByNick(params[3]);
                if (!target)
                    return (print_error(client.get_fd(), ERR_NOSUCHNICK(params[3])));
                if (!channel->is_client(*target) && !channel->is_operator(*target))
                    return (print_error(client.get_fd(), ERR_USERNOTINCHANNEL(params[3], params[1]) ));
                if (sign == '+') {
                    if (channel->is_client(*target))
                        channel->removeclient(*target);
                    channel->addoperator(*target);
                }
                else {
                    if (!channel->is_client(*target))
                        channel->addclient(*target);
                    channel->removeOperator(*target);
                    if ((channel->getOperators().empty()) && (channel->getClients().size()))
                    {
                        channel->getOperators().push_back(channel->getClients()[0]);
                        channel->getClients().erase(channel->getClients().begin());
                    }
                }
                break ;
            }
            default:
                valid = false;
                print_error(client.get_fd(), ERR_UNKNOWNMODE(client.get_nickname(), params[1], params[0]));
                break ;
        }
        if (valid)
        {
            appliedModes += sign;
            appliedModes += c;
            if (params.size() >= 4)
                appliedParams += " " + params[3];
        }
    }
    if (!appliedModes.empty()) {
        std::string mode_message = ":" + client.get_nickname()  + " MODE " + params[1] + " " + appliedModes + appliedParams + POSTFIX;
        channel->send_msg_in_channel(mode_message);
    }
} 