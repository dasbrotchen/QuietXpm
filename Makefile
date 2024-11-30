NAME = quietxpm

CC = cc

CFLAGS = -Wall -Werror -Wextra -g -I includes -MD -MP #-fsanitize=address

SRCS = main.c \
	   open_input_file.c \
	   qx_errors.c \
	   read_chunks.c \
	   parse_data_chunk.c \
	   print_xpm_content.c \
	   create_colortable.c \
	   fill_colortable.c \
	   free_utils.c

ZLIB_URL = https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz
ZLIB_DIR = zlib-1.3.1
ZLIB_BUILD_DIR = $(OBJ_D)zlib_build
ZLIB = $(ZLIB_BUILD_DIR)/libz.a
ZLIB_CFLAGS = -I$(ZLIB_BUILD_DIR)/include
ZLIB_LDFLAGS = -L$(ZLIB_BUILD_DIR) -lz

CFLAGS += $(ZLIB_CFLAGS)

INC = converter.h \
	  colortable.h

OBJS = $(SRCS:%.c=$(OBJ_D)%.o)

SRC_D = src/
OBJ_D = objects/
INC_D = includes/

$(OBJ_D)%.o: $(SRC_D)%.c
	@mkdir -p $(OBJ_D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJS) $(ZLIB)
	$(CC) $(CFLAGS) $(OBJS) $(ZLIB_LDFLAGS) -lm -o $@

$(ZLIB):
	@mkdir -p $(ZLIB_BUILD_DIR)
	@if [ ! -d "$(ZLIB_DIR)" ]; then \
		echo "Downloading zlib..."; \
		curl -L $(ZLIB_URL) | tar xz; \
	fi
	@cd $(ZLIB_DIR) && ./configure --static --prefix=$(shell pwd)/$(ZLIB_BUILD_DIR) && make test && make install

DEPS = $(OBJS:.o=.d)

all:
	@make --no-print-directory $(NAME)

-include $(DEPS)

clean:
	rm -rf $(OBJ_D)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
