#include "../includes/includes.hpp"
#include <fstream>
#include <iostream>

void Server::create_log_file(const std::string& filename)
{
    std::ofstream log_file(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!log_file.is_open()) {
        std::cerr << "Error: Unable to create log file." << std::endl;
        //must free the server data to avoid leaaks 
        exit(1);
    }
}