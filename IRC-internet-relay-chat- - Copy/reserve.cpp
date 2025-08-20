while (true)
{
    poll(fds.data(), fds.size(), 0);

    for (size_t i = 0; i < fds.size(); ++i)
    {
        if (fds[i].revents & POLLIN)
        {
            if (fds[i].fd == server_fd)
            {
                // 🔌 Accept new connection
                int client_fd = accept(...);
                send(client_fd, "WELCOME! Please enter password\n", ...);
                fds.push_back({client_fd, POLLIN, 0});
                clients.push_back({client_fd, false}); // not yet authenticated
            }
            else
            {
                // 🧍 Handle existing client
                int client_fd = fds[i].fd;
                char buffer[1024] = {0};
                int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
                if (bytes <= 0)
                {
                    // 🔌 Client disconnected
                    close(client_fd);
                    remove_client(fds, clients, client_fd);
                    continue;
                }

                std::string msg(buffer);

                t_client &client = find_client_by_fd(clients, client_fd);

                if (!client.authenticated)
                {
                    if (msg == password)
                    {
                        client.authenticated = true;
                        send(client_fd, "Authenticated!\n", ...);
                    }
                    else
                    {
                        send(client_fd, "Wrong password\n", ...);
                        // Optionally close connection
                    }
                }
                else
                {
                    // 💬 Parse message as command or chat
                    handle_command(msg, client_fd, clients, channels);
                }
            }
        }
    }
}
