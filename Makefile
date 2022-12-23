NAME = webserv
CC = c++
SRCS = srcs/main.cpp \
	   srcs/config.cpp \
	   srcs/server.cpp \
	   srcs/location.cpp \
	   srcs/socket.cpp \

FLAGS = -Wall -Wextra -Werror -g3
OBJS = $(SRCS:.cpp=.o)

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) ${FLAGS} $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all, clean, fclean, re
