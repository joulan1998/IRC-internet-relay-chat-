#include "../includes/includes.hpp"


Client* Server::getClientByNick(const std::string &nickname) {
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].get_nickname() == nickname)
            return &clients[i];
    }
    return NULL;
}

std::string int_to_string(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void Server::mode(Client &client, std::string &cmd)
{
    // validate that the user is logged in
    if (!client.get_registred() || !client.get_authenticated())
    {
        print_error(client.get_fd(), "ERR_NOTREGISTERED");
        return ;
    }
    // tokenize the input 
    std::vector<std::string> params = split(cmd, ' ', false);
    if (params.size() < 2)
    {
        print_error(client.get_fd(), "ERR_NEEDMOREPARAMS(\"MODE\")");
        return ;
    }
    std::string channelName = params[1];
    // validate the entered channel name and if it exists
    Channel *channel = getchannel(channelName);
    if (!channel)
    {
        print_error(client.get_fd(), "ERR_NOSUCHCHANNEL(\"channelName\")");
        return ;
    }
    // Here we just print the current modes associated with the channel
    if (params.size() == 2)
    {
        std::string modes = "+";
        std::string modeParams = "";

        if (channel->getFlag_i()) modes += "i";
        if (channel->getFlag_t()) modes += "t";
        if (channel->getFlag_k()) modes += "k";
        if (channel->getFlag_l())
        {
            modes += "l";
            modeParams = " " + int_to_string(channel->getLimit());
        }
        std::string modeMessage = PREFIX " MODE " + channelName + " " +  modes + modeParams + POSTFIX;
        print_error(client.get_fd(), modeMessage);
        return ; 
    }
    // if we don't only have two params like {MODE #channelName}, that means we are setting new modes and to do so the user would have to be an operator in the channel
    if (!channel->is_operator(client))
    {
        print_error(client.get_fd(), "ERR_CHANOPRIVSNEEDED(channelName)");
        return ;
    }
    // Now that we have validated that the user is an operator, let's go parsing the following new  modes:
    std::string modeString = params[2];
    size_t paramsIndex = 3;
    char sign = '+';
    std::string appliedModes = "";
    std::string appliedParams = "";
    for (size_t k = 0; k < modeString.length(); ++k)
    {
        char c  = modeString[k];
        if (c == '+' || c == '-')
        {
            sign = c;
            continue;
        }
        std::string modeParam = "";
        bool needsParam =  (sign == '+' && (c == 'k' || c == 'l' || c == 'o'))
                            || (sign == '-' && c == 'o');
        if (needsParam)
        {
            if (paramsIndex >= params.size()) 
            {
                print_error(client.get_fd(), "ERR_NEEDMOREPARAMS(\"MODE\")");
                return ;
            }
            modeParam = params[paramsIndex++];
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
                    channel->setPassword(modeParam);
                    channel->setFlag_k(true);
                }
                else
                {
                    channel->setPassword("");
                    channel->setFlag_k(false);
                }
                break;
            case 'l':
                if (sign == '+') {
                    channel->setLimit(string_to_int(modeParam));
                    channel->setFlag_l(true);
                }
                else {
                    channel->setLimit(0);
                    channel->setFlag_l(false);
                }
                break;
            case 'o': {
                Client *target = getClientByNick(modeParam);
                if (!target) {
                    print_error(client.get_fd(), "ERR_NOSUCHNICK(modeParam)");
                    return ;
                }
                if (!channel->is_client(*target) && !channel->is_operator(*target)) {
                    print_error(client.get_fd(), "ERR_USERNOTINCHANNEL(modeParam, channelName)");
                    return ;
                }
                if (sign == '+') {
                    if (channel->is_client(*target))
                        channel->removeOperator(*target);
                    channel->addoperator(*target);
                }
                else {
                    channel->removeOperator(*target);
                    if (!channel->is_client(*target))
                        channel->addclient(*target);
                }
                break ;
            }
            default:
                valid = false;
                print_error(client.get_fd(), "ERR_UMODEUNKNOWNFLAG(c)");
                break ;
        }
        if (valid) {
            appliedModes += sign;
            appliedModes += c;
            if (!modeParam.empty())
                appliedParams += " " + modeParam;
        }
    }
    if (!appliedModes.empty()) {
        std::string mode_message = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_host() + " MODE " + channelName + " " + appliedModes + appliedParams + POSTFIX;
        channel->send_msg_in_channel(mode_message);
    }
} 