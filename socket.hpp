#pragma once

#include <sys/socket.h>	//socket()
#include <netinet/in.h>	//htons()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()

#include "server.hpp"

class Socket
{
private:
	// attributes
	int	fd;

public:
	// listener constructor
	Socket(Server &server)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
		{
			//TODO handle errors
			// perror("Socket: socket");
		}
		if (bind(fd, (struct sockaddr *)&server.getAddress(), sizeof(server.getAddress())) < 0)
		{
			//TODO handle errors
			// perror("Socket: bind");
		}
		if (listen(fd, server.getBacklog()) == -1)
		{
			//TODO handle errors
			// perror("Socket: listen");
		}
	}

	// connected constructor
	Socket(ConnectedClient &newClient)
	{
		socklen_t socklen = sizeof(newClient.getAddress());
		newClient.getAddress().sin_family = AF_INET;
		fd = accept(newClient.getListeningFd(), (sockaddr *)&newClient.getAddress(), &socklen);
		if (fd == -1)
		{
			//TODO handle error
			// perror("Socket: accept")
		}
		struct epoll_event events;
		events.events = EPOLLIN;
		events.data.fd = newClient.getConnectedFd();
		if (epoll_ctl(newClient.getEpollFd(), EPOLL_CTL_ADD, newClient.getConnectedFd(), &events) == -1)
		{
			//TODO handle error
			// perror("Socket: epoll_ctl")
		}
	}

	// destructor
	~Socket()
	{
		if (fd != -1)
			close(fd);
	}

	// getter
	int const getFd() const { return fd; }

};
