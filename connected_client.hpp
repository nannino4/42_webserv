#pragma once

#include "base.hpp"
#include "server.hpp"
#include "socket.hpp"

class ConnectedClient : public Base
{
private:
	// attributes
	Server				&server;
	struct sockaddr_in	client_addr;
	char				buf[BUFSIZ];
	std::string			message;

public:
	// constructor
	ConnectedClient(Server &server) : server(server), message("")
	{
		bzero(&client_addr, sizeof(client_addr));
		bzero(buf, BUFSIZ);
		socket = new Socket(*this);
	}

	// destructor
	~ConnectedClient() {}

	// getters
	int const 			getListeningFd() const { return server.getListeningFd(); }
	int const 			getConnectedFd() const { return socket->getFd(); }
	struct sockaddr_in	&getAddress() { return client_addr; }
	int const			getKqueueFd() const { return server.getKqueueFd(); }
	char				*getBuf() { return buf; }
	std::string			&getMessage() { return message; }

	// communicate with server
	void communicate() { server.getRequest(*this); }

};
