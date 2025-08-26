#include "../includes/includes.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <fstream>

#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>

#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>

void Server::log_connection(Client& client_info)
{
    std::ofstream log_file("connection_history.log", std::ios::app);
    if (log_file.is_open())
    {
        // Get the current time
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
        // Format the time as a string
        char time_str[100];
        std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

        // Write to the log file using the client's username and IP address
        // Assuming your Client struct has a member for the username
        log_file << "[" << time_str << "] New client connected: " << client_info.get_username() << std::endl;
        puts("the reslt is written");
        log_file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open log file." << std::endl;
    }
}
// void Server::log_connection(Client& client_info)
// {
//     std::ofstream log_file("connection_history.log", std::ios::app);
//     if (log_file.is_open())
//     {
//         // Get the current time
//         std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
//         std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
//         // Format the time as a string
//         char time_str[100];
//         std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

//         // Write to the log file
//         log_file << "[" << time_str << "] New client connected: " << client_info.username << std::endl;
//         log_file.close();
//     }
//     else
//     {
//         std::cerr << "Error: Unable to open log file for writing." << std::endl;
//     }
// }
// void Server::log_connection(Client& client_info)
// {
//     std::ofstream log_file("connection_history.log", std::ios::app);
//     // std::ofstream log_file(LOG_FILE, std::ios::app);
//     if (log_file.is_open())
//     {
//         // Get the current time
//         std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
//         std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
//         // Format the time as a string
//         char time_str[100];
//         std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

//         // Write to the log file
//         log_file << "[" << time_str << "] New client connected: " << client_info.username << std::endl;
//         log_file.close();
//     }
//     else
//     {
//         std::cerr << "Error: Unable to open log file for writing." << std::endl;
//     }
// }
// void Server::log_connection(Client& client_info)
// {
//     std::ofstream log_file("connection_history.log", std::ios::app);
//     if (log_file.is_open())
//     {
//         // Get the current time
//         std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
//         std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
//         // Format the time as a string
//         char time_str[100];
//         std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

//         // Write to the log file
//         log_file << "[" << time_str << "] New client connected: " << client_info.username << std::endl;
//         log_file.close();
//     }
//     else
//     {
//         std::cerr << "Error: Unable to open log file." << std::endl;
//     }
// }

// void Server::log_connection(const std::string& client_info)
// {
//     std::ofstream log_file("connection_history.log", std::ios::app);
//     if (log_file.is_open())
//     {
//         // Get the current time
//         auto now = std::chrono::system_clock::now();
//         std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
//         // Format the time as a string
//         char time_str[100];
//         std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

//         // Write to the log file
//         log_file << "[" << time_str << "] New client connected: " << client_info << std::endl;
//         log_file.close();
//     }
//     else
//     {
//         std::cerr << "Error: Unable to open log file." << std::endl;
//     }
// }