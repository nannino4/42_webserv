#pragma once

#include <iostream>
#include <string>

#include <arpa/inet.h>	//inet_addr()
#include <unistd.h>		//close()
#include <string.h>		//bzero()
#include <ctime>		//struct timespec

#include "event.hpp"
#include "Request.hpp"
#include "Response.hpp"

struct ConnectedClient
{
	struct sockaddr_in	client_addr;
	int	const			connected_fd;
	Event				triggered_event;
	Request				request;
	Response			response;
	struct timespec		time_since_last_action;


	// constructor
	ConnectedClient(int const connected_fd, struct sockaddr_in client_addr, void *default_server_ptr);

	// copy constructor
	ConnectedClient(ConnectedClient const &other);

	// assign operator
	ConnectedClient &operator=(ConnectedClient const &other);

	// destructor
	~ConnectedClient();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, ConnectedClient const &client);

};
