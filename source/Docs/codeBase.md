# ft_irc codebase overview (what you need to implement MODE, INVITE, KICK, PRIVMSG)

This document summarizes the relevant types and flow so you can quickly plug in the remaining commands.

- Primary code path: [source/server.cpp](source/server.cpp), with headers in [includes/](includes/).
- Legacy/prototype path: [structred_irc/](structred_irc/) (useful for reference; the active server with channels and commands is under source/).

## Core types

- Client (class)
  - Header: [includes/client.hpp](includes/client.hpp)
  - Used fields (as seen in code): fd (socket), authenticated (bool), nickname (std::string), username (std::string).
  - You’ll see instances stored in `Server::clients` and passed around by reference.

- Channel (class)
  - Header: [includes/channel.hpp](includes/channel.hpp)
  - Known/used API (from call sites in [source/server.cpp](source/server.cpp)):
    - getName_channel()
    - addoperator(Client)
    - addclient(Client)
    - is_client(Client)
    - check_operator(Client)  // checks if client is channel operator
    - send_msg_in_channel(const std::string &)
    - setPassword(const std::string &), getPassword()
    - setFlag_k(bool)         // key (password) required
    - getFlag_k()
    - setFlag_l(bool)         // user limit enabled
    - getFlag_l()
    - setLimit(size_t), getLimit()
    - getOperators(), getClients()
    - list_of_client()        // used for RPL_NAMREPLY aggregation
    - getTopic(), setTopic(const std::string &)

- Server (class)
  - Header: [includes/server.hpp](includes/server.hpp)
  - Implementation: [source/server.cpp](source/server.cpp)
  - Sockets/state:
    - Holds the listening socket, `std::vector<pollfd> fds`, and `std::vector<Client> clients`.
  - Public methods used in the command loop:
    - [`Server::start_server`](source/server.cpp) — sets up sockets, poll loop, accepts clients, and dispatches commands.
    - Authentication (blocking, line-oriented):
      - [`Server::handle_password`](source/server.cpp)
      - [`Server::handle_nickname`](source/server.cpp)
      - [`Server::handle_username`](source/server.cpp)
      - [`Server::handle_new_client`](source/server.cpp)
    - Message/command handling:
      - [`Server::handle_message`](source/server.cpp) — generic recv/cleanup for authenticated clients.
      - [`Server::join`](source/server.cpp) — already implemented; a good template for KICK/INVITE/MODE policies.
      - [`Server::topic`](source/server.cpp) — already implemented; shows parsing and reply patterns.
      - [`Server::print_error`](source/server.cpp) — utility to send numeric replies/strings back to a client fd.
    - Channel management:
      - [`Server::getchannel`](source/server.cpp)
      - [`Server::addchannel`](source/server.cpp)
      - [`pars_join`](source/server.cpp) — helper for JOIN parsing (channels and keys lists).

## Runtime flow (high level)

1. Server bootstrap
   - [`Server::start_server`](source/server.cpp) calls socket setup routines (create/bind/listen), pushes the listening fd into `fds`, and adds a sentinel Client for the server socket.

2. Poll loop
   - When `POLLIN` on the server fd: accept new connection, push into `fds` and `clients`.
   - For an existing client:
     - If not authenticated, [`Server::handle_new_client`](source/server.cpp) runs:
       - [`Server::handle_password`](source/server.cpp) waits for: "PASS <password>"
       - [`Server::handle_nickname`](source/server.cpp) waits for: "NICK <nickname>"
       - [`Server::handle_username`](source/server.cpp) waits for: "USER <username>"
       - Sets `client.authenticated = true`.
     - If authenticated, `recv` and parse the line to dispatch commands.
       - Currently, the dispatcher in [`Server::start_server`](source/server.cpp) handles:
         - "join ..." → [`Server::join`](source/server.cpp)
         - "topic ..." → [`Server::topic`](source/server.cpp)
         - Unknown command → [`Server::print_error`](source/server.cpp) with ERR_UNKNOWNCOMMAND

3. Error/cleanup
   - [`Server::handle_message`](source/server.cpp) illustrates fd closure and erasing client/fd on disconnect or empty buffer.
   - [`Server::print_error`](source/server.cpp) is the unified path for numeric replies and messages.

Note: Authentication helpers use `ft_split` and a small `table_size` utility. Freeing of the allocated `char**` tables is noted as a TODO in the source; the legacy folder shows a `free_table` implementation and compatible `ft_split`.

## Utilities

- Legacy split/alloc helpers: 
  - `char **ft_split(const char*, char)` in [structred_irc/ft_split.cpp](structred_irc/ft_split.cpp)
  - `size_t table_size(char **table)` in [structred_irc/usrname_and_nickname.cpp](structred_irc/usrname_and_nickname.cpp)
  - A `free_table` exists in [structred_irc/server_set_up.cpp](structred_irc/server_set_up.cpp)
- Socket helpers (legacy style): [utils/irc_utils.cpp](utils/irc_utils.cpp)

These are referenced in the current `Server::handle_*` methods to parse simple space-delimited lines during auth.

## Where to hook your commands

- Dispatch location: inside the authenticated branch of [`Server::start_server`](source/server.cpp)
  - You will see the existing join/topic dispatch. Mirror that logic for:
    - "privmsg" → implement `Server::privmsg(Client, std::string)`
    - "invite" → implement `Server::invite(Client, std::string)`
    - "kick" → implement `Server::kick(Client, std::string)`
    - "mode" → implement `Server::mode(Client, std::string)`

- Suggested method placement:
  - Declarations in [includes/server.hpp](includes/server.hpp)
  - Implementations in [source/server.cpp](source/server.cpp)
  - Any channel-specific behavior (invited lists, operator checks, topic/modes) goes in [includes/channel.hpp](includes/channel.hpp) and [source/channel.cpp](source/channel.cpp)

## What JOIN/TOPIC tell you about expected behavior

Use JOIN and TOPIC as reference patterns:
- Input parsing:
  - JOIN uses [`pars_join`](source/server.cpp) to handle comma-separated channels/keys and normalizes them into pairs.
  - TOPIC uses a simple space split and handles ":"-prefixed trailing parameters (text after the channel name).
- Permission checks:
  - JOIN checks for channel existence; creates if missing, sets operator, enforces +k (key) and +l (limit).
  - TOPIC checks membership, channel existence, and whether topic is set.
- Replies:
  - Both use [`Server::print_error`](source/server.cpp) with macros like ERR_* and RPL_* and broadcast with `Channel::send_msg_in_channel`.
  - Your commands should follow the same reply patterns (target fd, numerics, and channel broadcasts where applicable).

## Command-specific notes

- PRIVMSG
  - Targets can be a nickname or a channel. You’ll need:
    - Resolve target client by nickname (add a helper in Server if not present).
    - Resolve target channel via [`Server::getchannel`](source/server.cpp).
    - Check sender is on the channel before sending to channel members.
    - Send to a single user (no echo back to sender), or broadcast to channel (exclude sender).
    - Errors: No such nick/channel, not on channel, no text to send.

- INVITE
  - Only channel operators should be able to invite (use `Channel::check_operator`).
  - The target nick receives an invite notification; the channel may need to keep an invited list if +i (invite-only) is implemented.
  - Errors: No such nick, no such channel, not channel operator, user already on channel.

- KICK
  - Only channel operators can kick. Remove the user from the channel’s clients list.
  - Broadcast the kick message to the channel.
  - Errors: No such channel, not on channel, not channel operator, no such user in channel.

- MODE
  - Channel modes likely include:
    - +k/-k (key): see `Channel::setPassword` and `setFlag_k`
    - +l/-l (limit): `setFlag_l`, `setLimit`
    - Possibly +i (invite-only) if you add it (header + impl).
  - Permission: Typically only operators can set channel modes.
  - Reply the new mode state and enforce it during JOIN and INVITE.

Tip: For all four commands, imitate the existing style in [`Server::join`](source/server.cpp) and [`Server::topic`](source/server.cpp): parse, validate, enforce permissions, apply to Channel, and reply/broadcast.

## Legacy folder (for reference only)

- Prototype server struct and flow under [structred_irc/](structred_irc/), e.g.:
  - `server` class in [structred_irc/inlcudes.hpp](structred_irc/inlcudes.hpp) and methods in:
    - [structred_irc/server_set_up.cpp](structred_irc/server_set_up.cpp)
    - [structred_irc/usrname_and_nickname.cpp](structred_irc/usrname_and_nickname.cpp)
    - [structred_irc/handle_password.cpp](structred_irc/handle_password.cpp)
    - [structred_irc/socket_handling.cpp](structred_irc/socket_handling.cpp)
  - These mirror what was re-implemented with Channels and commands in [source/](source/). Use them only as a parsing/flow reference.

## Minimal checklist to start

- Add command declarations to [includes/server.hpp](includes/server.hpp):
  - Server::privmsg(Client, std::string)
  - Server::invite(Client, std::string)
  - Server::kick(Client, std::string)
  - Server::mode(Client, std::string)
  - Optional helpers: resolve client by nickname; safe broadcast to channel.

- Add dispatch branches in [`Server::start_server`](source/server.cpp) next to JOIN/TOPIC.

- Extend Channel if needed (invited list, +i mode):
  - Add fields and accessors in [includes/channel.hpp](includes/channel.hpp) and implement in [source/channel.cpp](source/channel.cpp).

- Reuse [`Server::print_error`](source/server.cpp) for all numerics, and `Channel::send_msg_in_channel` for broadcasts.

With this, you can progress command-by-command and stay consistent with the existing style.