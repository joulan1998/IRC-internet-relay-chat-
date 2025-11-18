#include "../includes/includes.hpp"

void Server::handle_nickname(Client &local_client, std::vector<std::string> table)
{
    if (!local_client.get_nickname().empty())
        return print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
    if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return ;
    }
    else if (table.size() == 1)
    {
        std::string st= "NICK";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
    else if (table.size() != 2)
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_NONICKNAMEGIVEN(st));
        return;
    }
    else if ( !table[1].empty() && (check_nickname(table[1]) == 0))
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_NICKNAMEINUSE(st));
        return;
    }
    else if  (!table[1].empty())
    {
        local_client.set_nickname(table[1]);
        if (!local_client.get_nickname().empty() && !local_client.get_realname().empty() && !local_client.get_hostname().empty() && !local_client.get_servername().empty())
        {
            local_client.set_authenticated(true);
            print_error(local_client.get_fd(), RPL_WELCOME(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_YOURHOST(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_CREATED(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_MYINFO(local_client.get_nickname(), local_client.get_hostname()));
        }
        std::cout<<"Client <"<<local_client.get_fd()<<"> set NICK: "<< local_client.get_nickname() <<std::endl;
    }
    else 
        std::cout << "error nikname"<< std::endl;
}

void Server::handle_username(Client &local_client, std::vector<std::string> table)
{   
    if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return;
    }
    else if (local_client.get_registred() && (table.size() == 5) )
    {
        local_client.set_username(table[1]);
        local_client.set_hostname(table[2]);
        local_client.set_servername(table[3]);
        local_client.set_realname(table[4]);
        std::cout << "Client <" << local_client.get_fd() << "> set USER: "<<local_client.get_username()<< " HOST: "<<local_client.get_hostname()<< " SERV: "<<local_client.get_servername()<< " REAL: "<<local_client.get_realname()<< std::endl;
        if(!local_client.get_nickname().empty())
        {
            local_client.set_authenticated(true);
            print_error(local_client.get_fd(), RPL_WELCOME(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_YOURHOST(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_CREATED(local_client.get_nickname(), local_client.get_hostname()));
            print_error(local_client.get_fd(), RPL_MYINFO(local_client.get_nickname(), local_client.get_hostname()));
        }
    }
    else 
    {
        std::string st= "USER";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
}

void Server::handle_password(Client &local_client, std::string value)
{
    if(value.empty())
    {
        std::string st= "PASS";
        print_msg(local_client.get_fd(),  ERR_NEEDMOREPARAMS(st));
        return;
    }
    else if (local_client.get_registred())
        std::cout << "you are already passed the password !" << std::endl;
    else if(!strncmp(this->_password.c_str(), value.c_str(), value.size() + 1))
    {
        local_client.set_registred(true);
        std::cout << "Client <" << local_client.get_fd() << "> authenticated successfully." << std::endl;
    }
    else
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDMISMATCH(st));
        return;
    }
}

void Server::handle_new_client(Client &local_client, std::string buffer, size_t index)
{
    std::string temp_buff(buffer);
    std::vector<std::string> table = split(temp_buff, ' ' ,false);
    if (table.empty())
        return;
    to_upper(table[0]);
    if(temp_buff.empty())
    {
        close(local_client.get_fd());
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
        return;
    }
    else if (((!strncmp(table[0].c_str(), "PASS\0", 5))) && !local_client.get_registred())
        handle_password(local_client, table[1]);
    else if (((!strncmp(table[0].c_str(), "NICK\0", 5))) )
        handle_nickname(local_client, table);
    else if ((((!strncmp(table[0].c_str(), "USER\0", 5)))))
        handle_username(local_client, table);
    else if (!local_client.get_registred())
    {
        std::string st= "*";
        print_msg(local_client.get_fd(),  ERR_PASSWDINFIRST(st));
        return ;
    }
    else
        print_msg(local_client.get_fd(),  ERR_UNKNOWNCOMMAND(table[0]));
}
