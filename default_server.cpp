#include "default_server.hpp"

// constructor
DefaultServer::DefaultServer(int const &kqueue_fd, unsigned int backlog) : Server(kqueue_fd), backlog(backlog)
{
	bzero(buf, BUFFER_SIZE);
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	error_pages.insert(std::pair<int,std::string>(404, "./error_pages/404.html"));	//TODO aggiungi altre pagine di errore
	std::cout << "un nuovo default server e' stato creato" << std::endl;	//DEBUG
}

// destructor
DefaultServer::~DefaultServer()
{
	if (listening_fd != -1)
		close(listening_fd);
	std::cout << "un default server e' stato distrutto" << std::endl;		//DEBUG
}

// getters
struct sockaddr_in const	&DefaultServer::getAddress() const { return server_addr; }
unsigned int const			&DefaultServer::getBacklog() const { return backlog; }
int const					&DefaultServer::getListeningFd() const { return listening_fd; }
int							DefaultServer::getKqueueFd() const { return kqueue_fd; }

// communication
void DefaultServer::startListening()
{
	listening_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_fd < 0)
	{
		//TODO handle errors
		// perror("ERROR\nDefaultServer: socket");
	}
	if (bind(listening_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		//TODO handle errors
		// perror("ERROR\nDefaultServer: bind");
	}
	if (listen(listening_fd, backlog) == -1)
	{
		//TODO handle errors
		// perror("ERROR\nDefaultServer: listen");
	}
}

void DefaultServer::connectToClient()
{
	struct sockaddr_in	client_addr;
	int					connected_fd;

	bzero(&client_addr, sizeof(client_addr));
	socklen_t socklen = sizeof(client_addr);
	client_addr.sin_family = AF_INET;
	connected_fd = accept(listening_fd, (sockaddr *)&client_addr, &socklen);
	if (connected_fd == -1)
	{
		//TODO handle error
		// perror("ERROR\nDefaultServer: accept")
	}
	// create new ConnectedClient
	clients.insert(std::pair<int,ConnectedClient>(connected_fd, ConnectedClient(connected_fd, client_addr)));
	// add new connected_fd to kqueue for READ monitoring
	struct kevent event;
	EV_SET(&event, connected_fd, EVFILT_READ, EV_ADD, 0, 0, (void *)this);
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
	{
		//TODO handle error
		// perror("ERROR\nDefaultServer: adding connected_fd to kqueue with kevent()")
	}
}

void DefaultServer::receiveRequest(int const connected_fd)
{
	// find client corresponding to connected_fd
	if (clients.find(connected_fd) == clients.end())
	{
		//TODO handle error
		// strerror("ERROR\nDefaultServer: could not find connected_fd among clients")
	}
	ConnectedClient *client = &clients.find(connected_fd)->second;
	// read from connected_fd into client->message
	int read_bytes = recv(connected_fd, buf, BUFFER_SIZE, 0);
	if (read_bytes == -1)
	{
		//TODO handle error
		// perror("ERROR\nDefaultServer: receiveRequest: recv");
	}
	client->getMessage() += buf;
	bzero(buf, BUFFER_SIZE);
	if (read_bytes < BUFFER_SIZE)
		dispatchRequest(client);
}

void DefaultServer::dispatchRequest(ConnectedClient *client)
{
	//TODO dispatch the request to the corresponding server, based on the 'host' value
}
