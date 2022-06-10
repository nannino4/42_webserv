#pragma once

#include <string>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include "server.hpp"

#define BUFFER_SIZE BUFSIZ

class DefaultServer : public Server
{
private:
	// attributes
	// int const							&kqueue_fd;			// inherited from Server
	// std::map<int, std::string>			error_pages;
	// size_t								client_body_size;
	// std::map<std::string,Location>		locations;
	// std::map<int,ConnectedClients>		clients;
	unsigned int						backlog;
	// configuration parameters
	std::string							name;
	struct sockaddr_in					server_addr;
	std::map<std::string,Server*>		virtual_servers;
	// communication
	int									listening_fd;
	char								buf[BUFFER_SIZE];

public:
	// constructor
	DefaultServer(int const &kqueue_fd, unsigned int backlog);

	// destructor
	~DefaultServer();

	// getters
	struct sockaddr_in const	&getAddress() const;
	unsigned int	 const		&getBacklog() const;
	int const					&getListeningFd() const;
	int							getKqueueFd() const;

	// communication
	void startListening();
	void connectToClient();
	void receiveRequest(int const connected_fd);
	void dispatchRequest(ConnectedClient *client);

};
