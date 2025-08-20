#include "../includes/includes.hpp"
#include "../includes/client.hpp"

Client::Client(int fd, bool authenticated, std::string nickname, std::string username) : fd(fd), authenticated(authenticated), nickname(nickname), username(username) {}