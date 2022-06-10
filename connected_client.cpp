#include "connected_client.hpp"

ConnectedClient::ConnectedClient(int const &kqueue_fd, int const &listening_fd) :
kqueue_fd(kqueue_fd), listening_fd(listening_fd), message("")
{
	bzero(&client_addr, sizeof(client_addr));
	bzero(buf, BUFSIZ);
	socklen_t socklen = sizeof(client_addr);
	client_addr.sin_family = AF_INET;
	fd = accept(listening_fd, (sockaddr *)&client_addr, &socklen);
	if (fd == -1)
	{
		//TODO handle error
		// perror("ERROR\nSocket: accept")
	}
	struct kevent event;
	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, (void *)this);
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
	{
		//TODO handle error
		// perror("ERROR\nSocket: adding connectedFd to kqueue with kevent()")
	}
}

// destructor
ConnectedClient::~ConnectedClient()
{
	if (fd != -1)
		close(fd);
}

// getters
int const 					&ConnectedClient::getConnectedFd() const { return fd; }
struct sockaddr_in const	&ConnectedClient::getAddress() const { return client_addr; }
int const					&ConnectedClient::getKqueueFd() const { return kqueue_fd; }
char						*ConnectedClient::getBuf() { return buf; }
std::string					&ConnectedClient::getMessage() { return message; }

// communicate with server
void ConnectedClient::receiveRequest()
{
	int read_bytes = recv(fd, buf, BUFFER_SIZE, 0);
	if (read_bytes == -1)
	{
		//TODO handle error
		// perror("ERROR\nServer: readFromClient: recv");
	}
	message += buf;
	bzero(buf, BUFFER_SIZE);
	if (read_bytes < BUFFER_SIZE)
		sendResponse();
}

void ConnectedClient::sendResponse()
{
	std::cout << message << std::endl;	//DEBUG
	//TODO parse request ("message") and create response
}
