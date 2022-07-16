#include "connected_client.hpp"

// default constructor
ConnectedClient::ConnectedClient(int const connected_fd, struct sockaddr_in client_addr, void *default_server_ptr)
:	\
	client_addr(client_addr), \
	connected_fd(connected_fd), \
	triggered_event(this->connected_fd, default_server_ptr, this), \
	request(), \
	// response(),
	message(""), \
	message_pos(0) \
	{
		clock_gettime(CLOCK_BOOTTIME, &time_since_last_action);
	}

// copy constructor
ConnectedClient::ConnectedClient(ConnectedClient const &other) : connected_fd(other.connected_fd), triggered_event(other.triggered_event) { *this = other; }

// assign operator
ConnectedClient &ConnectedClient::operator=(ConnectedClient const &other)
{
	client_addr = other.client_addr;
	request = other.request;
	// response = other.response;
	message = other.message;
	message_pos = other.message_pos;
	time_since_last_action.tv_sec = other.time_since_last_action.tv_sec;
	return *this;
}

// destructor
ConnectedClient::~ConnectedClient() { close(connected_fd); }

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
