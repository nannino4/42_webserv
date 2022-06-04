#pragma once

#include <string>
#include <vector>

#include "cluster.hpp"
#include "location.hpp"
#include "base.hpp"
#include "connected_client.hpp"
#include "socket.hpp"

#define BUFFER_SIZE BUFSIZ

class Server : public Base
{
private:
	// attributes
	Cluster	const						&cluster;

	std::vector<std::string>			names;
	struct sockaddr_in					server_addr;
	bool								default_server;
	std::map<int, std::string>			error_pages;
	size_t								client_body_size;
	std::vector<Location>				locations;

	std::vector<ConnectedClient *>		clients;
	unsigned short						backlog;

public:
	// getters
	struct sockaddr_in const	&getAddress() const;
	unsigned short const		getBacklog() const
	int const					getListeningFd() const;
	int const					getKqueueFd() const;

	// constructor
	Server(Cluster const &cluster, bool default_server, unsigned short backlog);

	// destructor
	~Server()
	{
		for (std::vector<ConnectedClient *>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			(*it)->~ConnectedClient();
			delete *it;
		}
	}

	// start listening
	void startListening();

	// connect to client
	void connectToClient();

	// communicate with client
	void getRequest(ConnectedClient &client);
	void giveResponse(ConnectedClient &client);

};
