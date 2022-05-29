#pragma once

#include <string>
#include <vector>

#include "cluster.hpp"
#include "base.hpp"
#include "connected_client.hpp"
#include "socket.hpp"

class Server : public Base
{
private:
	// attributes
	Cluster	const					&cluster;
	std::vector<std::string>		names;
	struct sockaddr_in				server_addr;
	std::vector<ConnectedClient *>	clients_v;
	unsigned short					backlog;

public:
	// getters
	struct sockaddr_in const	&getAddress() const { return server_addr; }
	unsigned short const		getBacklog() const { return backlog; }
	int const					getListeningFd() const { return socket->getFd(); }
	int const					getEpollFd() const { return cluster.getEpollFd(); }

	// constructor
	Server(Cluster const &cluster, std::vector<std::string> names, struct sockaddr_in server_addr, unsigned short backlog) : cluster(cluster), names(names), server_addr(server_addr), backlog(backlog)
	{
		// bzero(&server_addr, sizeof(server_addr));
		// server_addr.sin_family = AF_INET;
		// server_addr.sin_port = htons(server.getPort());
		// server_addr.sin_addr.s_addr = inet_addr(server.getIpAddress().c_str());
	}

	// destructor
	~Server()
	{
		for (std::vector<ConnectedClient *>::iterator it = clients_v.begin(); it != clients_v.end(); ++it)
		{
			(*it)->~ConnectedClient();
			delete *it;
		}
	}

	// start listening
	void startListening()
	{
		socket = new Socket(*this);
	}

	// connect to client
	void connectToClient(void)
	{
		ConnectedClient *newClient = new ConnectedClient(*this);
		clients_v.push_back(newClient);
	}

};
