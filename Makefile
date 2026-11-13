NAME = ft_ping
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS	= src/main.c \
		  src/parse.c \
		  src/socket.c
OBJS = $(SRCS:.c=.o)
$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
clean:
		rm -f $(OBJS)
fclean: clean
		rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re