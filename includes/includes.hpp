#ifndef INCLUDES_H
#define INCLUDES_H

#include <cerrno>   // errno, EAGAIN, EWOULDBLOCK
#include <cstring>  // strerror, memset, strlen, strdup
#include <cstdio>   // perror, puts

#include <vector>
#include <sstream>
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

#define ERR_NOSUCHCHANNEL(channel)    PREFIX " 403 " + channel + " :No such channel" POSTFIX
#define ERR_BADCHANNELKEY(channel)    PREFIX " 475 " + channel + " :Cannot join channel (+k)" + POSTFIX
#define ERR_NEEDMOREPARAMS(client_name)    PREFIX " 461 " + client_name + " :Not enough parameters" POSTFIX  
#define ERR_NOTONCHANNEL(target, channel)    PREFIX "442 " + target + " " + channel + " :You're not on that channel" POSTFIX
#define ERR_CHANNELISFULL(client, channel)PREFIX " 471 " + client + " " + channel + " :Cannot join channel (+l)" POSTFIX
/////////////////
#define RPL_JOIN(sender, channel) ":" + sender + " JOIN :" + channel + POSTFIX
#define RPL_NAMREPLY(sender, channel, users)    PREFIX  " 353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel)        PREFIX  "366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
#define RPL_NOTOPIC(sender, channel)PREFIX " 331 " + sender + " " + channel + " :No topic is set" + POSTFIX
#define RPL_TOPIC(sender, channel, topic)PREFIX " 332 " + sender + " " + channel + " :" + topic + POSTFIX    

////////////////

class Server;



std::vector<std::string> split(std::string &str, char delem, bool flag);
void cleanString(std::string &s);

int string_to_int(const std::string &str);













#endif