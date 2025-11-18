#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Helper to send messages to IRC
void sendIRC(int sock, const std::string &msg)
{
    std::string full = msg + "\r\n";
    send(sock, full.c_str(), full.size(), 0);
}

// Get current local time as HH:MM:SS
std::string getCurrentTime()
{
    std::time_t t = std::time(NULL);
    std::tm* tm_info = std::localtime(&t);
    char buffer[9]; // HH:MM:SS
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    return std::string(buffer);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    std::string server = "127.0.0.1";   // change if remote server
    std::string nickname = "TimeBot";
    std::string channel  = "#weather";

    // --- Create socket ---
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server.c_str(), &addr.sin_addr) <= 0)
    { perror("inet_pton"); close(sock); return 1; }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    { perror("connect"); close(sock); return 1; }

    // --- IRC handshake ---
    sendIRC(sock, "PASS " + password);
    sleep(1);
    sendIRC(sock, "NICK " + nickname);
    sleep(1);
    sendIRC(sock, "USER " + nickname + " 0 * :" + nickname);
    sleep(1);
    sendIRC(sock, "JOIN " + channel);
    // puts("suzccesssss !!!!!!!!!!!!!!");
    // sendIRC(sock, "join #chan");
    // puts("suzccesssss !!!!!!!!!!!!!!");

    char buffer[4096];
    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        // std::cout << "TEST :::" << buffer << "::: TEST" << std::endl;
        // std::cout << "TEST :::" << buffer << "::: TEST" << std::endl;
        std::string msg(buffer);
        std::cout << msg;  // Print server messages

        // Respond to server PING
        // if (msg.find("PING") == std::string::npos)
        if (msg.find("PRIVMSG") != std::string::npos &&
            msg.find("PING") != std::string::npos)
        {
            std::string reply = "PRIVMSG " + channel + " PONG" + "\r";
            // std::string pong = "PONG" + msg.substr(5);
            sendIRC(sock, reply);
        }

        // Respond to !time command
        if (msg.find("PRIVMSG") != std::string::npos &&
            msg.find("!time\0") != std::string::npos)
        {
            std::string reply = "PRIVMSG " + channel + " :Current time: " + getCurrentTime() + "\r";
            // std::string reply = "PRIVMSG " + channel + " :Current time: " + getCurrentTime();
            // puts(reply.c_str());
            sendIRC(sock, reply);
            // exit(9);
        }
    }

    close(sock);
    return 0;
}
