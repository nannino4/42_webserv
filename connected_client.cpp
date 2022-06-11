#include "connected_client.hpp"

ConnectedClient::ConnectedClient(int const connected_fd, struct sockaddr_in client_addr) :
connected_fd(connected_fd), client_addr(client_addr), message(""), message_pos(0) {}

// destructor
ConnectedClient::~ConnectedClient()
{
	if (connected_fd != -1)
		close(connected_fd);
}
