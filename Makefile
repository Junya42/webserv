NAME = webserv

OBJ_DIR = objs
SRCS_DIR = $(shell find srcs -type d)

vpath %.cpp $(foreach dir, $(SRCS_DIR), $(dir))

SRCS = main.cpp \
	   config.cpp \
	   mime.cpp \
	   server.cpp \
	   location.cpp \
	   client.cpp \
	   request.cpp \
	   get_request.cpp \
	   string.cpp \
	   socket.cpp \
	   server_loop.cpp \

SRCS_PATH = $(addprefix srcs/, $(SRCS))
SRC_NUM := $(words $(SRCS))

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))
OBJ_LIST = $(wildcard objs/*.o)
DEPS = $(OBJS:%.o=%.d)

BASESRC = $(patsubst %.cpp,%,$(SRCS))
BASEOBJ = $(patsubst objs/%.o,%,$(OBJS))
CC = c++
FLAGS = -Wall -Wextra -Werror -g3

counter = 1
override counter := $(counter)

count := 0

all: assign_count $(NAME)

assign_count: compare-timestamps
	$(eval override count := $(shell cat .make_num))

compare-timestamps:
	@NUM=$(SRC_NUM); \
	for src in $(SRCS_PATH); \
	do \
		CURRSRC=$$(echo $$src | cut -d '/' -f2 | cut -d '.' -f1); \
		for obj in $(OBJ_LIST); \
		do \
			CURROBJ=$$(echo $$obj | cut -d '/' -f2 | cut -d '.' -f1); \
			if [ $$CURRSRC = $$CURROBJ ]; then \
				echo -n ""; \
				if [ $$src -nt $$obj ]; then \
					echo -n ""; \
				else \
					NUM=$$(expr $$NUM - 1); \
				fi \
			fi \
		done; \
	done; \
	echo $$NUM > .make_num

percent := 0
ascii := 0

range = 1 2 3 4 5 6 7 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30

define progress_bar
	$(eval override percent := $(shell echo "scale = 2; $(counter) / $(count) * 100" | bc))
	$(eval override ascii := $(shell echo "$(percent) / 10" | bc))
	$(eval override ascii := $(shell echo "$(ascii) * 3" | bc))
	clear
	@echo -n "["
	@for i in $(range); do \
		if [ $(ascii) -ge $$i ]; then \
			echo -n "█"; \
		else \
			echo -n "╍"; \
		fi \
	done; \
	echo "] $(percent)" 

endef

define print_message
	$(call progress_bar)
	$(eval override counter := $(shell expr $(counter) + 1))
endef

$(NAME): $(OBJS)
	@$(CC) ${FLAGS} $(OBJS) -o $(NAME)
	@echo "Successfully built $(NAME)"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "Compiling $@"
	@$(CC) $(FLAGS) -MMD -o $@ -c $<
	$(call print_message)

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "Removing objects and dependencies"
	@rm -rf $(OBJS) $(DEPS) $(OBJ_DIR)

fclean: clean
	@echo "Removing executable '$(NAME)'"
	@rm -rf $(NAME)

re: fclean
	$(MAKE) -C .

.PHONY: all, clean, fclean, re, $(OBJ_DIR), assign_count

-include $(DEPS)
