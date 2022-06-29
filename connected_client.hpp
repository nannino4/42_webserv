#pragma once

#include <iostream>
#include <string>

#include <arpa/inet.h>	//inet_addr()
#include <unistd.h>		//close()
#include <string.h>		//bzero()

#include "event.hpp"

struct ConnectedClient
{
	int	const			connected_fd;
	Event				triggered_event;
	struct sockaddr_in	client_addr;	
	std::string			message;
	int					message_pos;

	// constructor
	ConnectedClient(int const connected_fd, struct sockaddr_in client_addr, void *default_server_ptr);

	// destructor
	~ConnectedClient();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, ConnectedClient const &client);

};
