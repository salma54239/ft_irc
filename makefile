NAME = ircserv
NAME_BONUS = ircserv_bonus

## Sources ##
CC = c++
SRC =  Server/Server.cpp Server/Server_utils.cpp Client/Client.cpp Client/JOIN.cpp Server/main.cpp Server/Prompt.cpp Server/Utils.cpp Server/Authentication.cpp Client/Channel.cpp Client/PRIVMSG.cpp Client/TOPIC.cpp Client/MODE.cpp Client/INVITE.cpp Client/KICK.cpp
HEADER = Server/Server.hpp Client/Client.hpp Client/Channel.hpp
OBJ_F = $(SRC:%.cpp=%.o)
RM = rm -rf
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

## BONUS ##
SRC_BONUS =  bonus/IRCbot.cpp
HEADER_BONUS = bonus/IRCbot.hpp
OBJ_F_BONUS = $(SRC_BONUS:%.cpp=%.o)

## Rules ##
all: $(NAME)

$(NAME) : $(OBJ_F) $(HEADER)
	$(CC) $(CPPFLAGS) $(OBJ_F) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_F) $(OBJ_F_BONUS)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

bonus: $(NAME_BONUS)

$(NAME_BONUS) : $(OBJ_F_BONUS) $(HEADER_BONUS)
	$(CC) $(CPPFLAGS) $(OBJ_F_BONUS) -o $(NAME_BONUS)

.PHONY: all clean fclean re
