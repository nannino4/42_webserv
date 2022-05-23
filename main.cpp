#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>

#define PORT 8080

int main(int argc, char **argv)
{
	int					socket_fd;		// socket fd
	struct sockaddr_in	server_addr;	// struct containing internet address to be bound to socket

	// create socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}

	// initialize struct (containing internet address to be bound to socket) with host's address and a chosen port
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// bind internet address to socket
	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("ERROR binding socket");
		exit(EXIT_FAILURE);
	}

	// listen for clients willing to connect
	listen(socket_fd, 8);

	// connect and communicate with clients
	while (1)
	{
		// TODO leggi bene poll/epoll/select e scegli cosa usare e come usarlo
	}
}