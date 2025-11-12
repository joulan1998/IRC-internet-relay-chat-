#include "../includes/includes.hpp"

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
    std::string fullMessage = RPL_PRIVMSG(client.get_nickname(),client.get_username(), client.get_host(),target, message);
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