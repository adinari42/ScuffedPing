NAME = ft_ping
CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude
SRCS	= src/main.c \
		  src/parse.c
OBJS: $(NAME)
$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
clean:
		rm -f $(OBJS)
fclean: clean
		rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re