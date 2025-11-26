NAME = ircserv

BNS_NAME = bot_weather

SRC = main.cpp cmd/invite.cpp \
	source/ctrl_c_handling.cpp source/client_handling.cpp \
	source/kick.cpp source/channel.cpp source/server.cpp \
	utils/irc_utils.cpp source/client.cpp cmd/topic.cpp \
	cmd/join.cpp cmd/mode.cpp cmd/privmsg.cpp cmd/quit.cpp

BNS = bonus/bonus.cpp

INC = includes/includes.hpp includes/server.hpp includes/client.hpp includes/channel.hpp

OBJ = $(SRC:.cpp=.o)

OBJ_BNS = $(BNS:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

	
all: $(NAME)

$(NAME): $(OBJ) $(INC)
	$(CXX) $(CXXFLAGS)  $(OBJ) -o $(NAME) 


bonus: $(BNS_NAME)


$(BNS_NAME) : $(OBJ_BNS)
	$(CXX) $(CXXFLAGS)  $(OBJ_BNS) -o $(BNS_NAME) 

%.o: %.cpp $(INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OBJ_BNS)

fclean: clean
	rm -f $(NAME) $(BNS_NAME)

re: fclean all

.PHONY: all clean fclean re