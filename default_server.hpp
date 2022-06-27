#pragma once

#include <string>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <cstring>		//bzero
#include <unistd.h>		//close
#include <fcntl.h>		//fcntl

#ifdef mac // mac library
#include <sys/event.h>	//kqueue kevent
#endif
#ifdef __linux__ // linux library
#include <sys/epoll.h>  //epoll for linux
#endif

#include "server.hpp"

#define BUFFER_SIZE BUFSIZ

class DefaultServer : public Server
{
private:
	// int const							&kqueue_fd;			// inherited from Server
	// std::map<int, std::string>			error_pages;
	// size_t								client_body_size;
	// std::map<std::string,Location>		locations;


	typedef std::map<std::string, Server*>::iterator VirtualServerIterator;

	// attributes
	unsigned int						backlog;
	// configuration parameters
	std::string							name;
	struct sockaddr_in					server_addr;
	std::map<std::string,Server*>		virtual_servers;
	// communication
	std::map<int,ConnectedClient>		clients;
	int									listening_fd;
	char								buf[BUFFER_SIZE];

public:
	// constructor
	#ifdef mac
	DefaultServer(int const &kqueue_fd, unsigned int backlog);
	#endif
	#ifdef __linux__
	DefaultServer(int const &epoll_fd, unsigned int backlog);
	#endif
	// destructor
	~DefaultServer();

	// getters
	struct sockaddr_in const	&getAddress() const;
	unsigned int	 const		&getBacklog() const;
	int const					&getListeningFd() const;
	#ifdef mac
	int const					&getKqueueFd() const;
	#endif
	#ifdef __linux__
	int const					&getEpollFd() const;
	#endif
	// communication
	void startListening();
	void connectToClient();
	#ifdef mac
	void receiveRequest(struct kevent const event);
	#endif
	#ifdef __linux__
	void receiveRequest(struct epoll_event const event);
	#endif
	void dispatchRequest(ConnectedClient &client);
	// void prepareResponse(ConnectedClient &client);		// inherited from Server
	void sendResponse(int const connected_fd, int const buf_size);

};
