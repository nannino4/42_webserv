#pragma once

#include "server.hpp"
#include "socket.hpp"

class ConnectedClient
{
private:
	// attributes
	Server	&server;
	Socket	connected_socket;

};
