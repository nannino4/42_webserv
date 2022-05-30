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
	Server(Cluster const &cluster, std::vector<std::string> names, struct sockaddr_in server_addr, unsigned short backlog) : cluster(cluster), names(names), server_addr(server_addr), backlog(backlog) {}

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

	// communicate with client
	void getRequest(ConnectedClient &client)
	{
		int read_bytes = recv(client.getConnectedFd(), client.getBuf(), BUFSIZ, 0);
		if (read_bytes == -1)
		{
			//TODO handle error
			// perror("Server: readFromClient: recv");
		}
		client.getMessage() += client.getBuf();
		bzero(client.getBuf(), BUFSIZ);
		if (read_bytes < BUFSIZ)
			giveResponse(client);
	}

	void giveResponse(ConnectedClient &client)
	{
		std::cout << client.getMessage() << std::endl;	//DEBUG
		//TODO parse request - client.getMessage() - and create response
	}

};
