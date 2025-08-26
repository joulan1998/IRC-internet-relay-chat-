#include "../includes/includes.hpp"
#include "../includes/client.hpp"
// Client::Client( int fd;bool registred; bool authenticated; std::string nickname; std::string username; std::string hostname; std::string servername; std::string realname;) :   fd(fd), registred(registred),  authenticated(authenticated),  nickname(nickname),  username(username),  hostname(hostname), servername(servername), realname(realname);
Client::Client( int fd) : fd(fd), registred(false),  authenticated(false),  nickname(""),  username(""),  hostname(""), servername(""), realname(""){}

int Client::get_fd(void) {return this->fd;}
void Client::set_fd(int value){this->fd = value;}

void Client::set_registred(bool value){ this->registred = value;}
bool Client::get_registred(void){return this->registred;}

void Client::set_authenticated(bool value){ this->authenticated = value;}
bool Client::get_authenticated(void){return this->authenticated;}

void Client::set_nickname(std::string value){this->nickname = value;}
std::string Client::get_nickname(void){return this->nickname;}

void Client::set_username(std::string value){this->username = value;}
std::string Client::get_username(void){return this->username;}

void Client::set_hostname(std::string value){this->hostname = value;}
std::string Client::get_hostname(void){return this->hostname;}

void Client::set_servername(std::string value){this->servername = value;};
std::string Client::get_servername(void){return this->servername;};

void Client::set_realname(std::string value){this->realname = value;};
std::string Client::get_realname(void){return this->realname;}