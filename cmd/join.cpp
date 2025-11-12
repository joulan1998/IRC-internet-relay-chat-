#include "../includes/includes.hpp"



Channel* Server::getchannel(const std::string &name_channel)
{
    for(size_t i = 0; i < channels.size(); i++)
    {
        if (!name_channel.empty() && name_channel == channels[i].getName_channel())
            return &channels[i];
    }
    return NULL;
}

int Server::addchannel(Client _client, const std::string &name_channel)
{
    if (name_channel[0] == '#' && name_channel[1] != '\0')
        channels.push_back(Channel(name_channel));
    else
    {
        print_error(_client.get_fd(), ERR_NOSUCHCHANNEL(name_channel));
        return (1);
    }
    return 0;
}

void Server::print_error(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not send "<< std::endl;
}

std::vector<std::pair<std::string, std::string> > pars_join(std::vector<std::string> &cmd)
{
    std::vector<std::pair<std::string, std::string> > last_cmd;
    std::vector<std::string> keys;
    std::vector<std::string> channels;
    std::string password;
    if (cmd.size() > 2){keys = split(cmd[2], ',', true);}
    
    channels = split(cmd[1], ',', true);
    for(size_t j = 0; j < channels.size(); j++)
    {
        password = "";
        if (keys.size() > j)
            password = keys[j];
        last_cmd.push_back(std::make_pair(channels[j], password));
    }
    return last_cmd;

}

void Server::join(Client &client, std::vector<std::string> &cmd)
{
    if(cmd.size() < 2 )
    {
        print_error(client.get_fd(), ERR_NEEDMOREPARAMS(client.get_username()));
        return;
    }
    std::vector<std::pair<std::string, std::string> > ch_pass = pars_join(cmd) ;
    std::string msg;
    for (size_t i = 0; i < ch_pass.size(); i++)
    {
        Channel *ch = getchannel(ch_pass[i].first);
        if (ch && (ch->is_client(client) || ch->is_operator(client)))
        {
            print_error(client.get_fd(), ERR_USERONCHANNEL(ch->getName_channel(), client.get_nickname()));
            continue;
        }
        if(!ch)
        {
            if (ch_pass[i].first[0] != '#')
            {
                print_error(client.get_fd(), ERR_NOSUCHCHANNEL(ch_pass[i].first));
                continue;
            }
            if (addchannel(client, ch_pass[i].first) == 0)
            {
                ch = getchannel(ch_pass[i].first);
                ch->addoperator(client);
            }
        }
        if (ch)
        {
            if (ch->getFlag_i() && (ch->is_invited(client) == false))
            {
                std::cout << "Falg i  ===>  "<< ch->getFlag_i()<< std::endl;
                print_error(client.get_fd(), ERR_INVITEONLYCHAN(client.get_nickname(), ch->getName_channel()));
                continue;
            }
            if (ch->getFlag_l() && (ch->getLimit() == (ch->getOperators().size() + ch->getClients().size())))
            {
                print_error(client.get_fd(), ERR_CHANNELISFULL(client.get_nickname() ,ch->getName_channel()));
                continue;
            }
            if (ch->getFlag_k()  && ch->getPassword() != ch_pass[i].second)
            {
                print_error(client.get_fd(), ERR_BADCHANNELKEY(ch->getName_channel()));
                continue;
            }
            if(!ch->is_client(client))
            {
                if (!ch->is_operator(client))
                    ch->addclient(client);
                if (ch->is_invited(client))
                {
                    std::cout << "size I -->   " << ch->getInviteds().size() << std::endl;
                    ch->addclient(client);
                    std::vector<Client>::iterator it = ch->getInviteds().begin();
                    std::vector<Client>::iterator tmp = it;
                    tmp ++;
                    it = ch->getInviteds().erase(it); // erase returns new iterator
                    if (it != ch->getInviteds().end())
                        it  = tmp;
                    std::cout << "switch invited to a clientnormal    :::>"<< client.get_nickname() <<std::endl;

                    std::cout << "size II -->   " << ch->getInviteds().size() << std::endl;

                }
                ch->send_msg_in_channel(RPL_JOIN((client.get_nickname() + "!" + client.get_username() + "@" + client.get_host()),ch->getName_channel()));
                print_error(client.get_fd(), RPL_NAMREPLY(client.get_nickname(), ch->getName_channel(), ch->list_of_client()));
                print_error(client.get_fd(), RPL_ENDOFNAMES(client.get_nickname(), ch->getName_channel()));
                if (!ch->is_operator(client))
                {
                    if (ch->getTopic().empty())
                    {
                       print_error(client.get_fd(), RPL_NOTOPIC(client.get_nickname(), ch->getName_channel()));
                    }
                    else
                    {
                       print_error(client.get_fd(), RPL_TOPIC(client.get_nickname(), ch->getName_channel(), ch->getTopic()));
                       std::cout << ch->getTopic();
                    }
                }
            }
        }
    }
}
