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

	// copy constructor
	ConnectedClient(ConnectedClient const &other) : connected_fd(other.connected_fd), triggered_event(other.triggered_event) { *this = other; }

	// assign operator
	ConnectedClient &operator=(ConnectedClient const &other)
	{
		client_addr = other.client_addr;
		message = other.message;
		message_pos = other.message_pos;
		return *this;
	}

	// destructor
	~ConnectedClient();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, ConnectedClient const &client);

};
