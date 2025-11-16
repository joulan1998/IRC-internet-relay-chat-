#include "../includes/includes.hpp"

void Server::privmsg(Client &client, std::string &cmd)
{
    if (!client.get_registred() || !client.get_authenticated())
    {
        print_error(client.get_fd(), ERR_NOTREGISTERED());
        return;
    }
    std::string tmp = cmd;
    std::vector<std::string> params = split(tmp, ' ', false);
    if (params.size() < 2)
    {
        print_error(client.get_fd(), ERR_NORECIPIENT("(PRIVMSG)", client.get_nickname()));
        return;
    }
    std::string target = params[1];
    std::string message;
    if (params.size() > 2)
    {
        if (params[2][0] == ':' && params.size() > 3)
            message = std::string(cmd, cmd.find(':') + 1 , cmd.size() - cmd.find(':'));
        else
            message = std::string(params[2]);
    }
    if (message.empty())
    {
        print_error(client.get_fd(), ERR_NOTEXTTOSEND());
        return;
    }
    std::string fullMessage = RPL_PRIVMSG(client, target, message);
    if (!target.empty() && target[0] == '#')
    {
        Channel *chan = getchannel(target);
        if (!chan) {
            print_error(client.get_fd(), ERR_NOSUCHCHANNEL(target));
            return;
        }
        if (!chan->is_client(client) && !chan->is_operator(client))
        {
            print_error(client.get_fd(), ERR_CANNOTSENDTOCHAN(chan->getName_channel()));
            return;
        }
        chan->send_msg_in_channel(fullMessage);
        return;
    }
    Client *receiver = NULL;
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].get_fd() == this->_socket_fd)
            continue;
        if (clients[i].get_nickname() == target)
        {
            receiver = &clients[i];
            break;
        }
    }
    if (!receiver)
    {
        print_error(client.get_fd(), ERR_NOSUCHNICK(target));
        return;
    }
    print_error(receiver->get_fd(), fullMessage);
}