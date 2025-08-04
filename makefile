NAME = ircserv

SRC = main.cpp source/channel.cpp source/server.cpp utils/irc_utils.cpp

INC = includes/includes.hpp includes/server.hpp includes/client.hpp includes/channel.hpp

OBJ = $(SRC:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ) $(INC)
	$(CXX) $(CXXFLAGS)  $(OBJ) -o $(NAME) 

%.o: %.cpp $(INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re