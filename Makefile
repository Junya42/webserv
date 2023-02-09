NAME = webserv

OBJ_DIR = objs

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

DEBUG = 0

ifdef SET_DEBUG
	DSRCS_DIR = $(shell find d_srcs -type d)
	vpath %.cpp $(foreach dir, $(DSRCS_DIR), $(dir))
	SRCS_PATH = $(addprefix d_srcs/, $(SRCS))
	DEBUG = 1
else
	SRCS_DIR = $(shell find srcs -type d)
	vpath %.cpp $(foreach dir, $(SRCS_DIR), $(dir))
	SRCS_PATH = $(addprefix srcs/, $(SRCS))
endif

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))
OBJ_LIST = $(wildcard objs/*.o)
DEPS = $(OBJS:%.o=%.d)

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)
	@rm -rf /tmp/private_webserv
	@mkdir -p /tmp/private_webserv

$(NAME): $(OBJS)
	@$(CC) ${FLAGS} $(OBJS) -o $(NAME)
	@echo "Successfully built $(NAME)"
	@VAR=$(DEBUG); \
	ONE=1; \
	if [ $$VAR = $$ONE ]; then \
		echo "Debug version"; \
	fi

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "Compiling $@"
	@$(CC) $(FLAGS) -MMD -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $@

debug: fclean
	$(MAKE) SET_DEBUG=1

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
