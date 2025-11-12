#include "includes/includes.hpp"

Server *ref;

void f()
{
    system("leaks ircserv");
}

int main(int argc, char  **argv)
{
    atexit(f);
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