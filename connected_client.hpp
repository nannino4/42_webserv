#pragma once

#include <iostream>
#include <string>

#include <arpa/inet.h>	//inet_addr()
#include <unistd.h>		//close()

class ConnectedClient
{
private:
	// attributes
	int	const			connected_fd;
	struct sockaddr_in	client_addr;
	std::string			message;

public:
	// constructor
	ConnectedClient(int const connected_fd, struct sockaddr_in client_addr);

	// destructor
	~ConnectedClient();

	// getters
	int const 					&getConnectedFd() const;
	struct sockaddr_in const	&getAddress() const;
	std::string					&getMessage();

	// communicate with server
	void sendResponse();

};
