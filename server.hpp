#pragma once

#include <string>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include <sys/event.h>	//kqueue kevent

#include "location.hpp"
#include "connected_client.hpp"
#include "Response.hpp"
#include "Request.hpp"

#define DEF_404 "./error_pages/404.html"

class Server
{
protected:
	// attributes
	int const							&kqueue_fd;
	// configuration parameters
	std::vector<std::string>			names;
	std::map<int, std::string>			error_pages;
	size_t								client_body_size;
	std::map<std::string,Location>		locations;

public:
	// default constructor
	Server(int const &kqueue_fd);
	// copy constructor
	Server(Server const &other);
	// assign operator oveload
	Server &operator=(Server const &other);

	// destructor
	~Server();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, Server const &server);

	// getters
	int const					&getKqueueFd() const;
	std::vector<std::string> const	&getNames() const;

	// communication
	// void prepareResponse(ConnectedClient &client, void *default_server);
	void prepareResponse(ConnectedClient &client, const Request &request);

	// utility
	bool	isName(std::string const &name_to_match) const;

};
