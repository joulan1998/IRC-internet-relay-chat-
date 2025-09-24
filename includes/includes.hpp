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
#include <signal.h>
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
#define ERR_NEEDMOREPARAMS(command)    PREFIX "461 " + command + " :Not enough parameters" POSTFIX  
#define ERR_NOTONCHANNEL(target, channel)    PREFIX "442 " + target + " " + channel + " :You're not on that channel" POSTFIX
#define ERR_CHANNELISFULL(client, channel)PREFIX " 471 " + client + " " + channel + " :Cannot join channel (+l)" POSTFIX
#define ERR_UNKNOWNCOMMAND(command)    PREFIX "421 " + command + " :Unknown command" POSTFIX 
#define ERR_USERONCHANNEL(channel, target)    PREFIX " 443 " + channel + " " + target + " :is already on channel" POSTFIX
#define ERR_CHANOPRIVSNEEDED(channel)    PREFIX "482 " + channel + " :You're not channel operator" POSTFIX
#define ERR_INVITEONLYCHAN(client, channel)PREFIX " 473 " + client + " " + channel + " :Cannot join channel (+i)" POSTFIX
/////////////////

#define RPL_JOIN(sender, channel) ":" + sender + " JOIN :" + channel + POSTFIX
#define RPL_NAMREPLY(sender, channel, users)    PREFIX  " 353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel)        PREFIX  " 366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
#define RPL_NOTOPIC(sender, channel)PREFIX " 331 " + sender + " " + channel + " :No topic is set" + POSTFIX
#define RPL_TOPIC(sender, channel, topic)PREFIX " 332 " + sender + " " + channel + " :" + topic + POSTFIX    

////////////////

class Server;



std::vector<std::string> split(std::string &str, char delem, bool flag);
void cleanString(std::string &s);
// void my_sleep(unsigned int seconds);

int string_to_int(const std::string &str);


void to_upper(std::string &str);











#endif

