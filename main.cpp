#include "includes/includes.hpp"



Server *ref;
void f()
{
    close(ref->_socket_fd);
}


int main(int argc, char  **argv)
{
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
    atexit(f);
    // std::cout << server._port << server._password << std::endl;
    


}