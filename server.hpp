#pragma once

#include <string>
#include <vector>

#include "connected_client.hpp"
#include "socket.hpp"

class Server
{
private:
	// attributes
	Socket							*listening_socket;
	std::vector<std::string>		names;
	std::string						host;
	unsigned short					port;
	std::vector<ConnectedClient>	clients_v;
	unsigned short					backlog;

public:
	// getters
	unsigned short	getPort() { return port; }
	unsigned short	getBacklog() { return backlog; }
	std::string	getHost() { return host; }

	// constructor
	Server(std::vector<std::string> names, std::string host, unsigned short port, unsigned short backlog) : names(names), host(host), port(port), backlog(backlog) {}

	// destructor
	~Server()
	{
		listening_socket->~Socket();
		delete listening_socket;
	}

	// run
	void run()
	{
		listening_socket = new Socket(*this);
	}

};
