NAME = ircserv

SRC = main.cpp source/channel.cpp source/server.cpp utils/irc_utils.cpp
BNS = bonus/bonus.cpp bonus/create_file.cpp

INC = includes/includes.hpp includes/server.hpp includes/client.hpp includes/channel.hpp

OBJ = $(SRC:.cpp=.o)

OBJ_NBS = $(BNS:.cpp=.o)

CXX = c++

# FILE = connection_history.log

CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -std=c++98 -g

all: $(NAME)

$(NAME): $(OBJ) $(OBJ_BNS) $(INC) #$(FILE)
	$(CXX) $(CXXFLAGS) $(BNS)  $(OBJ) -o $(NAME) 

%.o: %.cpp $(INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OBJ_NBS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re