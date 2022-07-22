NAME		= 	webserv

SRCS		= 	main.cpp \
				cluster_init.cpp \
				cluster.cpp \
				connected_client.cpp \
				default_server_init.cpp \
				default_server.cpp \
				location_init.cpp \
				location.cpp \
				request.cpp \
				response.cpp \
				server.cpp \
				utils.cpp

OBJS		= 	$(SRCS:.cpp=.o)

CFLAGS		= 	-Wall -Wextra -Werror

CC			= 	g++

%.o :		%.cpp
			$(CC) -c -g -I. $(CFLAGS) $< -o $@

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(OBJS) -o $(NAME)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re