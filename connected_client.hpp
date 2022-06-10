#pragma once

#include <iostream>
#include <string>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include "base.hpp"

#define BUFFER_SIZE BUFSIZ

class ConnectedClient : public Base
{
private:
	// attributes
	int const			&kqueue_fd;
	int const			&listening_fd;
	struct sockaddr_in	client_addr;
	char				buf[BUFSIZ];
	std::string			message;

public:
	// constructor
	ConnectedClient(int const &kqueue_fd, int const &listening_fd);

	// destructor
	~ConnectedClient();

	// getters
	int const 					&getConnectedFd() const;
	struct sockaddr_in const	&getAddress() const;
	int const					&getKqueueFd() const;
	char						*getBuf();
	std::string					&getMessage();

	// communicate with server
	void receiveRequest();
	void sendResponse();

};
