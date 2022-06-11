#pragma once

#include <iostream>
#include <string>

#include <arpa/inet.h>	//inet_addr()
#include <unistd.h>		//close()
#include <string.h>		//bzero()

struct ConnectedClient
{
	int	const			connected_fd;
	struct sockaddr_in	client_addr;	
	std::string			message;
	int					message_pos;

	// constructor
	ConnectedClient(int const connected_fd, struct sockaddr_in client_addr);

	// destructor
	~ConnectedClient();

};
