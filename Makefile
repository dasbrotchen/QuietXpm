NAME = quietxpm

CC = cc

CFLAGS = -Wall -Werror -Wextra -g -I includes -MD -MP

SRCS = main.c \
	   open_input_file.c \
	   qx_errors.c \
	   read_chunks.c

INC = converter.h

Z_LIB_PATH = /opt/homebrew/Cellar/zlib/1.3.1/lib

OBJS = $(SRCS:%.c=$(OBJ_D)%.o)

SRC_D = src/
OBJ_D = objects/
INC_D = includes/

$(OBJ_D)%.o: $(SRC_D)%.c
	@mkdir -p $(OBJ_D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) -L$(Z_LIB_PATH) -lz $(OBJS) -o $@

DEPS = $(OBJS:.o=.d)

all:
	@make --no-print-directory $(NAME)

-include $(DEPS)

clean:
	rm -rf $(OBJ_D)

fclean:
	rm -rf $(OBJ_D)
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
