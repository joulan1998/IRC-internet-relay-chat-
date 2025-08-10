#include "inlcudes.hpp"
server *ref;
void f()
{
    close(ref->_socket_fd);
    // system("lsof -C ircserv");
}
int main(int argc, char  **argv)
{
    if (argc != 3)
    {
        std::cout << "the parama are not correct !" << std::endl;
        exit(1);
    }
        server serverf(argv[1], argv[2]);
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
    


}