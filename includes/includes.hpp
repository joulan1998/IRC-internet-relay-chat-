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


/////////////////


class Server;
std::vector<std::string> split(const char *str, char d);













#endif