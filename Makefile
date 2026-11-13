NAME	= ft_ping
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Iinclude
LDFLAGS	= -lm

SRCS	= src/main.c src/ping.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re