#include "connected_client.hpp"

ConnectedClient::ConnectedClient(int const connected_fd, struct sockaddr_in client_addr) :
connected_fd(connected_fd), client_addr(client_addr), message("") {}

// destructor
ConnectedClient::~ConnectedClient()
{
	if (connected_fd != -1)
		close(connected_fd);
}

// getters
int const 					&ConnectedClient::getConnectedFd() const { return connected_fd; }
struct sockaddr_in const	&ConnectedClient::getAddress() const { return client_addr; }
std::string					&ConnectedClient::getMessage() { return message; }

// communicate
void ConnectedClient::sendResponse()
{
	std::cout << message << std::endl;	//DEBUG
	//TODO parse request ("message") and create response
}
