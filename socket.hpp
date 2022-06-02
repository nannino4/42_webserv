#pragma once

#include <sys/socket.h>	//socket()
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
		struct kevent event;
		EV_SET(&event, newClient.getConnectedFd(), EVFILT_READ, EV_ADD, 0, 0, (void *)&newClient);
		if (kevent(newClient.getKqueueFd(), &event, 1, nullptr, 0, nullptr) == -1)
		{
			//TODO handle error
			// perror("Socket: adding connectedFd to kqueue with kevent()")
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
