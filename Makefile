NAME = lemipc
CC = gcc
CFLAGS = -Wall -Werror -Wextra -g
RM = rm -f
LIB = -lm -lftprintf

SRC = src/main.c src/game.c src/player.c src/ipc_conf.c src/parse.c src/cleanup.c \
		src/findenemy.c src/victory.c  src/auxiliar.c src/message_queue.c
OBJS = $(SRC:.c=.o)

FT_PRINTF_DIR = lib/printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

all: $(NAME)

$(NAME): $(OBJS) $(FT_PRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(FT_PRINTF_DIR) $(LIB)

$(FT_PRINTF):
	@make -C $(FT_PRINTF_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@make clean -C $(FT_PRINTF_DIR)

fclean: clean
	$(RM) $(NAME)
	@make fclean -C $(FT_PRINTF_DIR)
	@ipcrm -a

re: fclean all

.PHONY: all clean fclean re $(FT_PRINTF)