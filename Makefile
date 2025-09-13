NAME = lemipc
NAME_BONUS = visualizer
CC = gcc
CFLAGS = -Wall -Werror -Wextra -g
RM = rm -f
LIB = -lftprintf

SRC = src/main.c src/game.c src/player.c src/ipc_conf.c src/parse.c src/cleanup.c \
        src/findenemy.c src/auxiliar.c src/message_queue.c
BONUS_SRC = src/visualizer.c src/auxiliar.c
OBJS = $(SRC:.c=.o)
OBJS_BONUS = $(BONUS_SRC:.c=.o)

FT_PRINTF_DIR = lib/printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

all: $(NAME)

$(NAME): $(OBJS) $(FT_PRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(FT_PRINTF_DIR) $(LIB) -lm

bonus: re $(NAME_BONUS)

$(NAME_BONUS): $(OBJS_BONUS) $(FT_PRINTF)
	$(CC) $(CFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS) -L$(FT_PRINTF_DIR) $(LIB) -lm

$(FT_PRINTF):
	@make -C $(FT_PRINTF_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_BONUS)
	@make clean -C $(FT_PRINTF_DIR)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)
	@make fclean -C $(FT_PRINTF_DIR)
	@ipcrm -a

re: fclean all

.PHONY: all clean fclean re bonus $(FT_PRINTF)