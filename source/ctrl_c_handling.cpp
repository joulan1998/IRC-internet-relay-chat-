#include "../includes/includes.hpp"

void Server::ctr_c_handler(std::vector<Client> &new_cl, Client &client)
{
    std::vector<Client>::iterator it = new_cl.begin();

    while (it != new_cl.end())
    {
        if (it->get_fd() == client.get_fd()) 
        {
            std::vector<Client>::iterator tmp = it;
            tmp ++;
            it = new_cl.erase(it); 
            if (it != new_cl.end())
                it  = tmp;
        }
        else
            ++it;
    }
}

void Server::ctrl_c_handling(Client &client)
{

    
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::vector<Client> &ch_op = this->channels[i].getOperators();
        std::vector<Client> &cl = this->channels[i].getClients();
        ctr_c_handler(ch_op, client);
        ctr_c_handler(cl ,client);

        if (ch_op.empty() && cl.size())
        {
            ch_op.push_back(cl[0]);
            cl.erase(cl.begin());
            channels[i].send_msg_in_channel(RPL_UMODEIS(client.get_nickname(), this->channels[i].getName_channel(), "+o",ch_op[0].get_nickname()));
        }
    }
    for(size_t i = 0; i < channels.size(); i++)
    {
        if (channels[i].getClients().empty()  && channels[i].getOperators().empty())
        {
            if (channels[i].getInviteds().empty())
                this->channels.erase(this->channels.begin() + i);
            i--;
        }
    }
}