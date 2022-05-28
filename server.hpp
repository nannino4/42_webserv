#pragma once

#include <string>
#include <vector>

#include "base.hpp"
#include "connected_client.hpp"
#include "socket.hpp"

class Server : public Base
{
private:
	// attributes
	std::vector<std::string>		names;
	std::string						ipAddress;
	unsigned short					port;
	std::vector<ConnectedClient>	clients_v;
	unsigned short					backlog;

public:
	// getters
	unsigned short const	getPort() const { return port; }
	unsigned short const	getBacklog() const { return backlog; }
	std::string const		&getIpAddress() const { return ipAddress; }
	Socket const			&getListeningSocket() const { return *socket; }

	// constructor
	Server(std::vector<std::string> names, std::string ipAddress, unsigned short port, unsigned short backlog) : names(names), ipAddress(ipAddress), port(port), backlog(backlog) {}

	// destructor
	~Server()
	{
		socket->~Socket();
		delete socket;
	}

	// start listening
	void startListening()
	{
		socket = new Socket(*this);
	}

};
