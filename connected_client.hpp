#pragma once

#include "base.hpp"
#include "server.hpp"
#include "socket.hpp"

class ConnectedClient : public Base
{
private:
	// attributes
	Server const		&server;
	struct sockaddr_in	client_addr;

public:
	// constructor
	ConnectedClient(Server const &server) : server(server)
	{
		bzero(&client_addr, sizeof(client_addr));
		socket = new Socket(*this);
	}

	// destructor
	~ConnectedClient() {}

	// getters
	int const 			getListeningFd() const { return server.getListeningFd(); }
	int const 			getConnectedFd() const { return socket->getFd(); }
	struct sockaddr_in	&getAddress() { return client_addr; }
	int const			getEpollFd() const { return server.getEpollFd(); }

};
