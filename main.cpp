#include "includes/includes.hpp"

Server *ref;

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
        // puts("before ");
        // serverf.create_log_file(LOG_FILE);
        serverf.create_log_file("connection_history.log");
        // puts("after ");
        serverf.start_server();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    


}