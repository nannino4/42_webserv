#pragma once

#include <string>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()
#ifdef __MACH__
#include <sys/event.h>	//kqueue kevent
#endif
#ifdef __linux__
#include <sys/epoll.h>  //epoll for linux
#endif
// #include "location.hpp"
#include "connected_client.hpp"
#include "Response.hpp"
#include "Request.hpp"

class Server
{
protected:
	// attributes
	#ifdef __MACH__
	int const							&kqueue_fd;
	#endif
	#ifdef __linux__
	int const							&epoll_fd;
	#endif
	// configuration parameters
	std::map<int, std::string>			error_pages;
	size_t								client_body_size;
	// std::map<std::string,Location>		locations;

public:
	// constructor
	#ifdef __MACH__
	Server(int const &kqueue_fdg);
	#endif
	#ifdef __MACH__
	Server(int const &epoll_fdg);
	#endif
	// destructor
	~Server();

	// getters
	#ifdef __MACH__
	int const	&getKqueueFd() const;
	#endif
	#ifdef __linux__
	int const	&getEpollFd() const;
	#endif
	// communication
	// void prepareResponse(ConnectedClient &client, void *default_server);
	void prepareResponse(ConnectedClient &client, const Request & request);

};
