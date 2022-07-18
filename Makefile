NAME		= 	webserv

SRCS		= 	main.cpp \
				cluster.cpp \
				cluster_init.cpp \
				default_server.cpp \
				default_server_init.cpp \
				server.cpp \
				connected_client.cpp \
				response.cpp \
				request.cpp \
				location.cpp \
				location_init.cpp \
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