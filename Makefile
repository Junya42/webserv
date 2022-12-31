NAME = nwebserv
NAME_O = webserv

CC = c++
SRCS = srcs/main.cpp \
	   srcs/config.cpp \
	   srcs/server.cpp \
	   srcs/location.cpp \
	   srcs/client.cpp \
	   srcs/request.cpp \
	   srcs/string.cpp \
	   srcs/socket.cpp \

SRCS_O = srcs/main.cpp \
	   srcs/config.cpp \
	   srcs/server.cpp \
	   srcs/location.cpp \
	   srcs/client.cpp \
	   srcs/request.cpp \
	   srcs/oldsocket.cpp \

FLAGS = -Wall -Wextra -Werror -g3
OBJS = $(SRCS:.cpp=.o)
OBJS_O = $(SRCS_O:.cpp=.o)

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) ${FLAGS} $(OBJS) -o $(NAME)

old: $(NAME_O)

$(NAME_O): $(OBJS_O)
	$(CC) ${FLAGS} $(OBJS_O) -o $(NAME_O)

clean:
	rm -rf $(OBJS)
	rm -rf $(OBJS_O)

fclean: clean
	rm -rf $(NAME)
	rm -rf $(NAME_O)

re: fclean all

.PHONY: all, clean, fclean, re
