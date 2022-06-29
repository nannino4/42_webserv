#include "connected_client.hpp"

ConnectedClient::ConnectedClient(int const connected_fd, struct sockaddr_in client_addr, void *default_server_ptr)
: connected_fd(connected_fd), triggered_event(connected_fd, default_server_ptr, this), client_addr(client_addr), message(""), message_pos(0) {}

// destructor
ConnectedClient::~ConnectedClient() {}

// operator overload
std::ostream &operator<<(std::ostream &os, ConnectedClient const &client)
{
	os << "\nConnectedClient introducing itself:\n";
	os << "connected_fd:\n" << client.connected_fd << std::endl;
	os << "server_addr:\n" << inet_ntoa(client.client_addr.sin_addr) << ":" << ntohs(client.client_addr.sin_port) << std::endl;
	os << "message:\n" << client.message << std::endl;
	os << "message_pos:\n" << client.message_pos << std::endl;
	os << "\nConnectedClient introduction is over" << std::endl;
	return os;
}
