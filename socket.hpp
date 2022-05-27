#pragma once

#include <sys/socket.h>	//socket()
#include <netinet/in.h>	//htons()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include "server.hpp"

class Socket
{
private:
	// attributes
	int	fd;

public:
	// listener constructor
	Socket(Server &server)
	{
		struct sockaddr_in	server_addr;
		// initialize attributes
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
		{
			//TODO handle errors
			// perror("ERROR opening socket");
			// exit(EXIT_FAILURE);
		}
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(server.getPort());
		server_addr.sin_addr.s_addr = inet_addr(server.getHost().c_str());
		if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		{
			//TODO handle errors
			// perror("ERROR binding socket");
			// exit(EXIT_FAILURE);
		}
		listen(fd, server.getBacklog());
	}

	// connected constructor
	Socket()
	{
		//TODO connected_client_socket
	}

	// destructor
	~Socket() { close(fd); }

};
