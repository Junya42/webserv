NAME = webserv

OBJ_DIR = objs
SRCS_DIR = $(shell find srcs -type d)

vpath %.cpp $(foreach dir, $(SRCS_DIR), $(dir))

SRCS = main.cpp \
	   config.cpp \
	   mime.cpp \
	   error.cpp \
	   server.cpp \
	   location.cpp \
	   client.cpp \
	   request.cpp \
	   get_request.cpp \
	   string.cpp \
	   socket.cpp \
	   server_loop.cpp \
	   auto_index.cpp \
	   cgi.cpp	\
	   cgi2.cpp

SRCS_PATH = $(addprefix srcs/, $(SRCS))

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))
OBJ_LIST = $(wildcard objs/*.o)
DEPS = $(OBJS:%.o=%.d)

CC = c++
FLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) ${FLAGS} $(OBJS) -o $(NAME)
	@echo "Successfully built $(NAME)"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "Compiling $@"
	@$(CC) $(FLAGS) -MMD -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "Removing objects and dependencies"
	@rm -rf $(OBJS) $(DEPS) $(OBJ_DIR)
	@rm -rf /tmp/private_webserv

fclean: clean
	@echo "Removing executable '$(NAME)'"
	@rm -rf $(NAME)

re: fclean
	$(MAKE) -C .

.PHONY: all, clean, fclean, re, $(OBJ_DIR)

-include $(DEPS)
