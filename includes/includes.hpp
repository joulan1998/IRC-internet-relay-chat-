#ifndef INCLUDES_H
#define INCLUDES_H

#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sys/socket.h>
#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"

//////////////////
#define PREFIX ":IRCServer10"
#define POSTFIX "\r\n"

#define ERR_NOSUCHCHANNEL(channel)    PREFIX "403 " + channel + " :No such channel" POSTFIX
#define ERR_BADCHANNELKEY(channel)    PREFIX "475 " + channel + " :Cannot join channel (+k)" + POSTFIX

/////////////////
#define RPL_JOIN(sender, channel) (std::string(":")) + sender + " JOIN :" + channel + POSTFIX
#define RPL_NAMREPLY(sender, channel, users)    PREFIX  "353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel)        PREFIX std::string "366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
////////////////

class Server;
std::vector<std::string> split(const char *str, char d);













#endif