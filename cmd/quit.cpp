#include "../includes/includes.hpp"

void Server::quit_handler(std::vector<Client> &new_cl, Client &client, std::vector<std::string> &new_cmd, size_t &i, std::string &cmd)
{
    std::string reasen;
    std::vector<Client>::iterator it = new_cl.begin();

    while (it != new_cl.end())
    {
        if (it->get_fd() == client.get_fd()) 
        {
            std::vector<Client>::iterator tmp = it;
            tmp ++;
            it = new_cl.erase(it); // erase returns new iterator
            if (it != new_cl.end())
                it  = tmp;
            (void)i;
            (void)cmd;
            size_t index = cmd.find(new_cmd[0]) + new_cmd[0].length();
            // //check for find
            while(cmd[index] == ' ') index++;           
            if (cmd[index] != ':')
            {
                std::string str = "";
                if (new_cmd.size() > 2)
                    str = new_cmd[2];
                reasen = str + POSTFIX;
                
            }
            else if (cmd[index] == ':')
            {
                index++;
                reasen = &cmd[index];
            }
            channels[i].send_msg_in_channel(RPL_QUIT((client.get_nickname() + "!" + client.get_username() + "@" + client.get_host()), reasen));
        }
        else
            ++it;
    }
}

void Server::quit(Client &client, std::string &cmd)
{
    std::vector<std::string> new_cmd = split(cmd, ' ', false);
    
    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::vector<Client> &ch_op = this->channels[i].getOperators();
        std::vector<Client> &cl = this->channels[i].getClients();
        quit_handler(ch_op, client,new_cmd,i,cmd);
        quit_handler(cl ,client,new_cmd,i,cmd);

        if (ch_op.empty() && cl.size())
        {
            ch_op.push_back(cl[0]);
            cl.erase(cl.begin());
            channels[i].send_msg_in_channel(RPL_UMODEIS(client.get_nickname(), this->channels[i].getName_channel(), "+o",ch_op[0].get_nickname()));
        }
    }
    close(client.get_fd());
    
}
