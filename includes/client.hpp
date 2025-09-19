#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>


class Client
{
    private:
        int fd;
        bool registred;
        bool authenticated;
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string servername;
        std::string realname;
        std::string host;
    public:
        // Client( int fd ,bool registred, bool authenticated, std::string nickname, std::string username, std::string hostname, std::string servername, std::string realname);
        Client( int fd );
        void set_fd(int value);
        int get_fd(void);

        void set_authenticated(bool value);
        bool get_authenticated(void);

        void set_registred(bool value);
        bool get_registred(void);

        void set_nickname(std::string value);
        std::string get_nickname(void);

        void set_username(std::string value);
        std::string get_username(void);

        void set_hostname(std::string value);
        std::string get_hostname(void);

        void set_servername(std::string value);
        std::string get_servername(void);

        void set_realname(std::string value);
        std::string get_realname(void);

        void set_host(std::string value);
        std::string get_host(void);
};


#endif