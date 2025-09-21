#include "includes/includes.hpp"

Server *ref;

int main(int argc, char  **argv)
{
    puts("in order to be able to request and receive the response for (ping or time) from the bot you should be also in the #weather channel < PRIVMSG #weather : !time > OR < PRIVMSG TimeBot : !time>");
    if (argc != 3)
    {
        std::cout << "the parama are not correct !" << std::endl;
        exit(1);
    }
        Server serverf(argv[1], argv[2]);
        ref = &serverf;
    try
    {
        serverf.start_server();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    


}