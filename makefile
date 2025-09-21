NAME     = ircserv
BONUS    = bot_weather

SRC      = main.cpp source/channel.cpp source/server.cpp utils/irc_utils.cpp source/client.cpp#_getters_setters.cpp source/client.cpp
BNS      = bonus/bot_weather.cpp

INC      = includes/includes.hpp includes/server.hpp includes/client.hpp includes/channel.hpp

OBJ      = $(SRC:.cpp=.o)
OBJ_BNS  = $(BNS:.cpp=.o)

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -std=c++98 -g

# Default: build server only
all: $(NAME)

# Build the server executable
$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

# Build the bot executable
bonus: $(BONUS)

$(BONUS): $(OBJ_BNS)
	$(CXX) $(CXXFLAGS) $(OBJ_BNS) -o $(BONUS) -lcurl

# Generic rule to compile object files
%.o: %.cpp $(INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove object files only
clean:
	rm -f $(OBJ) $(OBJ_BNS)

# Remove object files + executables
fclean: clean
	rm -f $(NAME) $(BONUS)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re bonus
# NAME = ircserv

# SRC = main.cpp source/channel.cpp source/server.cpp utils/irc_utils.cpp source/client.cpp

# INC = includes/includes.hpp includes/server.hpp includes/client.hpp includes/channel.hpp

# OBJ = $(SRC:.cpp=.o)

# CXX = c++

# CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -std=c++98 -g

# all: $(NAME)

# $(NAME): $(OBJ) $(INC)
# 	$(CXX) $(CXXFLAGS)  $(OBJ) -o $(NAME) 

# %.o: %.cpp $(INC)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJ)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re