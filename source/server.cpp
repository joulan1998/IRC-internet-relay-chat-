#include  "../includes/server.hpp"
#include  "../includes/channel.hpp"
#include "../includes/includes.hpp"
Server* Server::instance = NULL;
Server *ref;
// the function for closing the fd
void test_fun(int sig)
{
    (void) sig;
    size_t i =0;
    while (i > ref->clients.size())
    {
        close(ref->clients[i].get_fd());
        i++;
    }
    close(ref->_socket_fd);
    
    std::cout << ref->_socket_fd << std::endl;
    return;
}

void Server::handle_nickname(Client &local_client, std::string value)
{
    // if (!local_client.registred)
    if (!local_client.get_registred())
        std::cout << "please enter password in first !"<< std::endl;
    else if (value.empty())
        return;
    else if  (!value.empty())
        local_client.set_nickname(value);
        // local_client.nickname = value;
    else 
        std::cout << "error nikname"<< std::endl;
}

void Server::handle_username(Client &local_client, std::vector<std::string> table)
{   
    if ((table.size() != 5))
    {
        std::cout << "error : use <USER username hostname servername realname> !"<< std::endl;
    }
    else if (table[1].size() >= 4 && local_client.get_registred())
    {
        local_client.set_username(table[1]);
        local_client.set_hostname(table[2]);
        local_client.set_servername(table[3]);
        local_client.set_realname(table[4]);
        local_client.set_authenticated(true);
    }
    else 
        std::cout << "error username : username's size < 4 "<< std::endl;
}


void Server::create_socket()
{
    int local_socket;
    local_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (local_socket < 0 )
    {
        throw(std::runtime_error("socket_error" + std::string(strerror(errno))));
    }
    this->_socket_fd = local_socket;
}

void Server::set_non_blocking()
{
    fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK);
}

void    Server::socket_options()
{
    int option = 1;
    if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR ,&option , sizeof(option)) < 0)
    {
        perror("SET SOCKOPT ERROR : ");
        exit(1);
    }
}


void Server::set_socket_addr()
{
    this->_socket_addr = new sockaddr_in;
    if (!this->_socket_addr)
         throw(std::runtime_error("allocation_error : " + std::string(strerror(errno))));
    this->_socket_addr->sin_family = AF_INET;
    this->_socket_addr->sin_port = htons(this->_port);;
    this->_socket_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
}

Server::Server(std::string port, std::string password)
{
    this->_password = password;
    this->_port = string_to_int(port);
    // this->_port = std::stoi(port);
}


void Server::set_listen()
{
    if (listen(this->_socket_fd, MAX_CLIENT) < 0)
    {
        throw(std::runtime_error("listen_error : " + std::string(strerror(errno))));
    }
}

void Server::bind_server()
{
    if (bind(this->_socket_fd, (struct sockaddr *)(this->_socket_addr), sizeof(sockaddr)) < 0)
    {
        throw(std::runtime_error("bind_error : " + std::string(strerror(errno))));
    }
}

void Server::handle_password(Client &local_client, std::string value/*,size_t index*/)
{
    if(value.empty())
        return;
    // else if (local_client.registred)
    else if (local_client.get_registred())
        std::cout << "you are already passed the password !" << std::endl;
    else if(!strncmp(this->_password.c_str(), value.c_str(), value.size()))
    {
        // local_client.registred = true;
        local_client.set_registred(true);
        // log_connection(local_client);
    }
    else 
        std::cout << "handle password erroor  !" << std::endl;
}



// void Server::handle_new_client(Client &local_client)
void Server::handle_new_client(Client &local_client, std::string buffer, size_t index)
{
    std::string temp_buff(buffer);
    std::vector<std::string> table = split(temp_buff, ' ' ,false);
    if (table.empty())
        return;
    if(temp_buff.empty())
    {
        exit(94);
        // close(local_client.fd);
        close(local_client.get_fd());
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
        return;
    }
    else if ((!strncmp(table[0].c_str(), "PASS\0", 5)) || (!strncmp(table[0].c_str(), "pass\0", 5)))
    {
        handle_password(local_client, table[1]/*, index*/);
    }
    else if ((!strncmp(table[0].c_str(), "NICK\0", 5)) || (!strncmp(table[0].c_str(), "nick\0", 5)))
        handle_nickname(local_client, table[1]);
    else if (((!strncmp(table[0].c_str(), "USER\0", 5)) || (!strncmp(table[0].c_str(), "user\0", 5)))/* && (table.size() == 5)*/)
    {
        handle_username(local_client, table);   
    }
    else
        puts("unkown command during auth !");
}



void    Server::handle_message(Client &local_client, char *buffer, int index)
{
    // std::cout << local_client.fd<<std::endl;
    std::cout << local_client.get_fd()<<std::endl;
    std::cout << (int)*buffer << "  :" << buffer;
    if (!*buffer)
    {
        // close(local_client.fd);
        close(local_client.get_fd());
        this->clients.erase(this->clients.begin() + index);
        this->fds.erase(this->fds.begin() + index);
    }
    std::cout << "Client " << local_client.get_fd() << " :" << buffer;
    // std::cout << "Client " << local_client.fd << " :" << buffer;

}



void Server::start_server()
{
    create_socket();
    set_non_blocking();
    set_socket_addr();
    socket_options();
    bind_server();
    set_listen();
    //    instance = this;
    ref = this;
    signal(SIGINT, test_fun);
    signal(SIGQUIT, test_fun);

    this->fds.push_back((pollfd){this->_socket_fd, POLLIN, 0});
    this->clients.push_back(Client(this->_socket_fd)); // <<<<<< should set REGISTRED to true

    puts(this->_password.c_str());
    size_t i =0;
    ssize_t bytes_readen =0;
    char buffer[1024];
    while(1)
    {
        int client_fd;
        if (poll(fds.data(), fds.size(), -1) < 0)
            throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
            // puts("poll_error");
        for (i = 0; i < fds.size(); i++)
        {
            if (this->fds[i].revents & POLLIN)
            {
                sockaddr client_addr;
                socklen_t client_len = sizeof(client_addr);
                if (this->fds[i].fd == this->_socket_fd)
                {
                    client_fd = accept(this->_socket_fd, &client_addr, &client_len);
                    this->fds.push_back((pollfd){client_fd, POLLIN, 0});
                    this->clients.push_back(Client(client_fd));
                    // this->clients.push_back((Client){client_fd, false, false, "", "", "", "", ""});
                }
                else
                {
                    Client &local_client = this->clients[i];
                    //hadechi bach nejib host deyal wahed l user ///// /<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<------------------------------__________----------------------
                    sockaddr_in* addr_in = (sockaddr_in*)&client_addr;
                    char ip_str[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
                    std::string client_ip(ip_str);
                    local_client.set_host(client_ip);
                    // std::cout<< "--------- >>>>>> {"<<client_ip<<"} <<<<<< ----------"<< std::endl<<std::endl;
                    fcntl(local_client.get_fd(), F_SETFL, O_NONBLOCK);
                    memset(buffer, 0,1024);
                    bytes_readen = recv(local_client.get_fd(), buffer,1024, 0);
                    if (bytes_readen == 0 && clients.size() > 1)
                    {
                        puts("jojooojo");
                        std::cout << clients.size()<< std::endl;
                        this->clients.erase(this->clients.begin() + i);
                        close((this->fds.begin() + i)->fd);

                        // close(this->fds[this->fds.begin() + i].fd);
                        this->fds.erase(this->fds.begin() + i);
                        continue;
                    }
                    else if (bytes_readen < 0 )
                        throw(std::runtime_error("poll_error : " + std::string(strerror(errno))));
                    if (!local_client.get_authenticated() && local_client.get_fd() != this->_socket_fd)
                    {
                        handle_new_client(local_client, buffer, i);
                        send(local_client.get_fd(), "PRIVMSG : TimeBot !time", 24, 0);

                    }
                    else 
                    {
                        std::string new_buffer(buffer);
                        pars_cmd(buffer, local_client);
                    }
                }
            }
        }

    }

}

void Server::pars_cmd(std::string buffer, Client &local_client)
{
    std::vector<std::string> split_buffer = split(buffer, ' ', false);
    // puts(buffer.c_str());
    if (!split_buffer.size()) return;
    to_upper(split_buffer[0]);
    if (split_buffer.size() && split_buffer[0] == "JOIN")
        join(local_client, split_buffer);
    else if (split_buffer.size() && split_buffer[0] == "TOPIC")
        topic(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "PRIVMSG")
        privmsg(local_client, buffer);
    else if (split_buffer.size() && split_buffer[0] == "MODE")
        mode(local_client, buffer);
    // else if (split_buffer.size() && split_buffer[0] == "QUIT")
        // quit(local_client, buffer);
    else if(split_buffer.size())
        print_error(local_client.get_fd(),ERR_UNKNOWNCOMMAND(split_buffer[0]) );
    
}


//////////////channel
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

std::vector<std::pair<std::string, std::string> > pars_join(std::vector<std::string> &cmd)
{
    std::vector<std::pair<std::string, std::string> > last_cmd;
    std::vector<std::string> keys;
    std::vector<std::string> channels;
    std::string password;
    if (cmd.size() > 2){keys = split(cmd[2], ',', true);}
    
    channels = split(cmd[1], ',', true);
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::cout << channels[i] << std::endl;
    }
    for(size_t j = 0; j < channels.size(); j++)
    {
        password = "";
        if (keys.size() > j)
            password = keys[j];
        last_cmd.push_back(std::make_pair(channels[j], password));
    }
    for (size_t i = 0; last_cmd.size() > i; i++)
        std::cout << last_cmd[i].first << "  ==  " << last_cmd[i].second << std::endl;
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
            
            if (addchannel(client, ch_pass[i].first) == 0)
            {
                ch = getchannel(ch_pass[i].first);
                ch->addoperator(client);
                if (ch_pass[i].second.empty())//TODO hada tah ghir tmp bach n testi bih
                {
                    ch->setPassword(ch_pass[i].second);
                    ch->setFlag_k(true);
                }
                ch->setFlag_l(true);
                ch->setLimit(2);
            }
        }

        if (ch)
        {
            if (ch->getFlag_i() && !(ch->is_invited(client)))
            {
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
            if(!ch->is_client(client) )
            {
                if (!ch->is_operator(client))
                    ch->addclient(client);

                ch->send_msg_in_channel(RPL_JOIN(client.get_nickname(),ch->getName_channel()));
                print_error(client.get_fd(), RPL_NAMREPLY(client.get_nickname(), ch->getName_channel(), ch->list_of_client()));
                print_error(client.get_fd(), RPL_ENDOFNAMES(client.get_nickname(), ch->getName_channel()));

            }

        
        }
    }
}
// void Server::join(Client &client, std::vector<std::string> &cmd)
// {
//     if(cmd.size() < 2 )
//     {
//         print_error(client.get_fd(), ERR_NEEDMOREPARAMS(client.get_username()));
//         return;
//     }
//     std::vector<std::pair<std::string, std::string> > ch_pass = pars_join(cmd) ;
//     std::string msg;
//     for (size_t i = 0; i < ch_pass.size(); i++)
//     {
//         Channel *ch = getchannel(ch_pass[i].first);
//         if (ch && (ch->is_client(client) || ch->is_operator(client)))
//         {
//             print_error(client.get_fd(), ERR_USERONCHANNEL(ch->getName_channel(), client.get_nickname()));
//             continue;
//         }
//         if(!ch)
//         {
//             if (ch_pass[i].first[0] != '#')
//             {
//                 print_error(client.get_fd(), ERR_NOSUCHCHANNEL(ch_pass[i].first));
//                 continue;;
//             }
//             if (addchannel(client, ch_pass[i].first) == 0)
//             {
//                 ch = getchannel(ch_pass[i].first);
//                 ch->addoperator(client);
//                 if (ch_pass[i].second.empty())//TODO hada tah ghir tmp bach n testi bih
//                 {
//                     ch->setPassword(ch_pass[i].second);
//                     ch->setFlag_k(true);
//                 }
//                 ch->setFlag_l(true);
//                 ch->setLimit(2);
//             }
//         }

//         if (ch)
//         {
//             // if (ch->getFlag_i() && !(ch->is_invited(client)))
//             // {
//             //     print_error(client.get_fd(), ERR_INVITEONLYCHAN(client.get_nickname(), ch->getName_channel()));
//             //     continue;
//             // }
//             if (ch->getFlag_l() && (ch->getLimit() == (ch->getOperators().size() + ch->getClients().size())))
//             {
//                 print_error(client.get_fd(), ERR_CHANNELISFULL(client.get_nickname() ,ch->getName_channel()));
//                 continue;
//             }
//             if (ch->getFlag_k()  && ch->getPassword() != ch_pass[i].second)
//             {
//                 print_error(client.get_fd(), ERR_BADCHANNELKEY(ch->getName_channel()));
//                 continue;
//             }
//             if(!ch->is_client(client) )
//             {
//                 if (!ch->is_operator(client))
//                     ch->addclient(client);

//                 ch->send_msg_in_channel(RPL_JOIN((client.get_nickname() + "!" + client.get_username() + "@" + client.get_host()),ch->getName_channel()));
//                 print_error(client.get_fd(), RPL_NAMREPLY(client.get_nickname(), ch->getName_channel(), ch->list_of_client()));
//                 print_error(client.get_fd(), RPL_ENDOFNAMES(client.get_nickname(), ch->getName_channel()));
//                 if (!ch->is_operator(client))
//                 {
//                     if (ch->getTopic().empty())
//                     {
//                        print_error(client.get_fd(), RPL_NOTOPIC(client.get_nickname(), ch->getName_channel()));
//                     }
//                     else
//                     {
//                        print_error(client.get_fd(), RPL_TOPIC(client.get_nickname(), ch->getName_channel(), ch->getTopic()));
//                        std::cout << ch->getTopic();
//                     }
//                 }
                    

//             }
//         }
//     }
// }




///////////////////////////////////////////////////////////////////////////

void Server::print_error(int fd, std::string msg)
{
    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "msg not send "<< std::endl;
}


//TODO KHASNI NZID ILA CHANNEL KHAS GHIR ADMIN LI BDEL TOPIC
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
            //new
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















// ==================> frenzy commands

void Server::privmsg(Client &client, std::string &cmd)
{
    // Require full registration (PASS/NICK/USER)
    if (!client.get_registred() || !client.get_authenticated()) {
        print_error(client.get_fd(), "ERR_NOTREGISTERED");
        return;
    }

    // Tokenize command safely (work on a copy; split mutates input)
    std::string tmp = cmd;
    std::vector<std::string> params = split(tmp, ' ', false);
    if (params.size() < 2) {
        print_error(client.get_fd(), "ERR_NORECIPIENT(\"PRIVMSG\")");
        return;
    }

    std::string target = params[1];

    // Extract message text: prefer trailing param after ':', else join remaining tokens
    std::string message;
    size_t colonPos = cmd.find(':');
    if (colonPos != std::string::npos) {
        message = cmd.substr(colonPos + 1);
    } 
    else if (params.size() > 2) {
        for (size_t j = 2; j < params.size(); ++j) {
            message += params[j];
            if (j + 1 < params.size()) message += "";
        }
    }
    if (message.empty()) {
        print_error(client.get_fd(), "ERR_NOTEXTTOSEND");
        return;
    }

    // Build the full IRC line
    std::string fullMessage = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_host() + " PRIVMSG " + target + " :" + message + POSTFIX;

    // Channel target
    if (!target.empty() && target[0] == '#') {
        Channel *chan = getchannel(target);
        if (!chan) {
            print_error(client.get_fd(), ERR_NOSUCHCHANNEL(target));
            return;
        }
        if (!chan->is_client(client) && !chan->is_operator(client)) {
            print_error(client.get_fd(), "ERR_CANNOTSENDTOCHAN(target)");
            return;
        }
        chan->send_msg_in_channel(fullMessage);
        return;
    }

    // Nickname target: resolve inline
    Client *receiver = NULL;
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].get_fd() == this->_socket_fd)
            continue;
        if (clients[i].get_nickname() == target) {
            receiver = &clients[i];
            break;
        }
    }
    if (!receiver) {
        print_error(client.get_fd(), "ERR_NOSUCHNICK(target)");
        return;
    }
    print_error(receiver->get_fd(), fullMessage);
}


Client* Server::getClientByNick(const std::string &nickname) {
    for (size_t i = 0; i < clients.size(); ++i) {
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
