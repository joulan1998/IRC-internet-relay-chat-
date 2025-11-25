
#ifndef INCLUDES_H
#define INCLUDES_H

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#define PREFIX ":IRCServer10"
#define POSTFIX "\r\n"
#define ERR_NOSUCHCHANNEL(channel)    PREFIX " 403 " + channel + " :No such channel" POSTFIX
#define ERR_BADCHANNELKEY(channel)    PREFIX " 475 " + channel + " :Cannot join channel (+k)" + POSTFIX
#define ERR_NEEDMOREPARAMS(command)    PREFIX " 461 " + command + " :Wrong number of parameters" POSTFIX  
#define ERR_NOTONCHANNEL(target, channel)    PREFIX " 442 " + target + " " + channel + " :You're not on that channel" POSTFIX
#define ERR_CHANNELISFULL(client, channel)PREFIX " 471 " + client + " " + channel + " :Cannot join channel (+l)" POSTFIX
#define ERR_UNKNOWNCOMMAND(command)    PREFIX " 421 " + command + " :Unknown command" POSTFIX 
#define ERR_USERONCHANNEL(channel, target)    PREFIX " 443 " + channel + " " + target + " :is already on channel" POSTFIX
#define ERR_CHANOPRIVSNEEDED(channel)    PREFIX " 482 " + channel + " :You're not channel operator" POSTFIX
#define ERR_INVITEONLYCHAN(client, channel)PREFIX " 473 " + client + " " + channel + " :Cannot join channel (+i)" POSTFIX
#define RPL_JOIN(sender, channel) ":" + sender + " JOIN :" + channel + POSTFIX
#define RPL_NAMREPLY(sender, channel, users)    PREFIX  " 353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel)        PREFIX  " 366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
#define RPL_NOTOPIC(sender, channel)PREFIX " 331 " + sender + " " + channel + " :No topic is set" + POSTFIX
#define RPL_TOPIC(sender, channel, topic)PREFIX " 332 " + sender + " " + channel + " :" + topic + POSTFIX
#define RPL_QUIT(sender, reason) ":" + sender + " QUIT : " + reason + POSTFIX
#define RPL_UMODEIS(hostname, channelname, mode, user)":" + hostname + " MODE " + channelname + " " + mode + " " + user + POSTFIX
#define ERR_NOTREGISTERED() PREFIX " 451 :You have not registered" POSTFIX
#define ERR_NORECIPIENT(command, sender) PREFIX + sender + " 411 :No recipient given " command POSTFIX
#define ERR_NOTEXTTOSEND() PREFIX " 412 :No text to send" POSTFIX
#define ERR_CANNOTSENDTOCHAN(channel) PREFIX  " 404"+ channel + ":Cannot send to channel" POSTFIX
#define RPL_PRIVMSG(client, target, msg)  ":" + (client).get_nickname() + "!" + (client).get_username() + "@" + (client).get_host() + " PRIVMSG " + (target) + " :" + (msg) + POSTFIX
#define RPL_INVITE(sender, target, channel)": " + sender + " INVITE " + target + " " + channel + POSTFIX
#define RPL_INVITING(nickname, targnick, targchan)  ": 341 " + nickname + " " + targnick + " " + targchan + POSTFIX
#define RPL_WELCOME(nick, hostname)  std::string("\r\n:") + std::string(hostname) + std::string(" 001 ") + std::string(nick) + std::string(" :Welcome to the Internet Relay Chat Network ") + std::string(nick) + "!~" + std::string(nick) + "@" + std::string(hostname) + "\r\n"
#define RPL_YOURHOST(nick, hostname) std::string(":") + std::string(hostname) + std::string(" 002 ") + std::string(nick) + std::string(" :Your host is ") + std::string(hostname) + std::string(", running version 1.0\r\n")
#define RPL_CREATED(nick, hostname)  std::string(":") + std::string(hostname) + std::string(" 003 ") + std::string(nick) + std::string(" :This server is created today\r\n")
#define RPL_MYINFO(nick, hostname)   std::string(":") + std::string(hostname) + std::string(" 004 ") + std::string(nick) + " " + std::string(hostname) + std::string(", Version: 1.0, User mode: none, Channel modes: i, t, k, o, l\r\n")
#define ERR_NICKNAMEINUSE(target)    PREFIX " 433 " + target + " :Nickname is already in use" POSTFIX
#define ERR_PASSWDMISMATCH(target)    PREFIX " 464 " + target + " :Password incorrect" POSTFIX //password incorroct 
#define ERR_PASSWDINFIRST(target)    PREFIX " 464 " + target + " :Password required before other commands" POSTFIX //password incorroct 
#define ERR_NONICKNAMEGIVEN(target) PREFIX "431 " + target + " :No nickname given" POSTFIX
#define ERR_NOSUCHNICK(nickname)    PREFIX " 401 " + nickname + " :No such nick/channel" POSTFIX
#define RPL_KICK(kikker, target, channel, reason)    ":" +  kikker +" KICK " + channel + " " + target + " :" + reason + POSTFIX
#define RPL_CHANGEMODE(hostname, channelname, mode)(":" + hostname + " MODE " + channelname + " " + mode + POSTFIX)
#define ERR_NOMODE(nick, channelname)(":" + nick + " MODE " + channelname + " No modes to set "  + POSTFIX)
#define ERR_UNKNOWNMODE(nickname, channelname, mode)(std::string(": 472 ") + nickname + " " + channelname + " " + mode + " :is not a recognised channel mode" + POSTFIX)
#define ERR_USERNOTINCHANNEL(user, channel)     PREFIX "441 " + user + " " + channel + " :They aren't on that channel" POSTFIX
class Server;

std::vector<std::string> split(std::string &str, char delem, bool flag);
void cleanString(std::string &s);



void to_upper(std::string &str);

#endif