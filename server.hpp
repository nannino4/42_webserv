#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include "base.hpp"
#include "connected_client.hpp"
#include "location.hpp"

class Server : public Base
{
private:
	// attributes
	int const		&kqueue_fd;
	unsigned int	backlog;

	std::string							name;
	struct sockaddr_in					server_addr;
	std::map<int, std::string>			error_pages;
	size_t								client_body_size;
	std::map<std::string,Location>		locations;

	std::map<std::string,Server*>		virtual_servers;
	std::vector<ConnectedClient*>		clients;

public:
	// constructor
	Server(int const &kqueue_fd, unsigned int backlog);

	// destructor
	~Server();

	// getters
	struct sockaddr_in const	&getAddress() const;
	unsigned int	 const		&getBacklog() const;
	int const					&getListeningFd() const;
	int							getKqueueFd() const;

	// start listening
	void startListening();

	// connect to client
	void connectToClient();

};
