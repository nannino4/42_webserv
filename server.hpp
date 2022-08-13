#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>	//stat

#ifdef __MACH__
	#include <sys/event.h>	//kqueue kevent
#elif defined(__linux__)
	#include <sys/epoll.h>  //epoll for linux
#endif

#include "location.hpp"
#include "connected_client.hpp"
#include "Cgi.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

class Server
{
protected:
	// attributes
	int const							&kqueue_epoll_fd;
	// configuration parameters
	std::vector<std::string>			names;
	std::map<int,std::string>			error_pages;
	size_t								client_body_size;
	std::map<std::string,Location>		locations;
	Location							default_location;

public:
	// default constructor
	Server(int const &kqueue_epoll_fd);
	// copy constructor
	Server(Server const &other);
	// assign operator oveload
	Server &operator=(Server const &other);

	// destructor
	~Server();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, Server const &server);

	// getters
	int const								&getKqueueEpollFd() const;
	std::vector<std::string> const			&getNames() const;
	std::map<int,std::string> const			&getErrorPages() const;
	size_t const							&getClientBodySize() const;
	std::map<std::string,Location> const	&getLocations() const;

	// communication
	void	prepareResponse(ConnectedClient *client);
private:
	void	methodGet(ConnectedClient *client);
	void	methodPost(ConnectedClient *client);
	void	methodDelete(ConnectedClient *client);

	// utility
public:
	bool	isName(std::string const &name_to_match) const;
private:
	void	fileToBody(ConnectedClient *client);
	void	manageDir(ConnectedClient *client);
	void	generateAutoIndex(Request &request, Response &response);
protected:
	void	errorPageToBody(Response &response);
};
