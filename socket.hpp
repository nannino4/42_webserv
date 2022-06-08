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
	Socket(Server &server);

	// connected constructor
	Socket(ConnectedClient &newClient);

	// destructor
	~Socket();

	// getter
	int const getFd() const;

};
