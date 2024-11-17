NAME = quietxpm

CC = cc

CFLAGS = -Wall -Werror -Wextra -g -I includes -MD -MP

SRCS = main.c

INC = converter.h

OBJS = $(SRCS:%.c=$(OBJ_D)%.o)

SRC_D = src/
OBJ_D = objects/
INC_D = includes/

$(OBJ_D)%.o: $(SRC_D)%.c
	@mkdir -p $(OBJ_D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

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
