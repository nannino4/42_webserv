#include "server.hpp"

// constructor
Server::Server(int const &kqueue_fd, unsigned int backlog) : kqueue_fd(kqueue_fd), backlog(backlog)
{
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	error_pages.insert(std::pair<int,std::string>(404, "./error_pages/404.html"));	//TODO aggiungi altre pagine di errore
	std::cout << "un nuovo server e' stato creato" << std::endl;	//DEBUG
}

// destructor
Server::~Server()
{
	for (std::vector<ConnectedClient *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->~ConnectedClient();
		delete *it;
	}
	if (fd != -1)
		close(fd);
	std::cout << "un server e' stato distrutto" << std::endl;		//DEBUG
}

// getters
struct sockaddr_in const	&Server::getAddress() const { return server_addr; }
unsigned int const			&Server::getBacklog() const { return backlog; }
int const					&Server::getListeningFd() const { return fd; }
int							Server::getKqueueFd() const { return kqueue_fd; }

// start listening
void Server::startListening()
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		//TODO handle errors
		// perror("ERROR\nSocket: socket");
	}
	if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		//TODO handle errors
		// perror("ERROR\nSocket: bind");
	}
	if (listen(fd, backlog) == -1)
	{
		//TODO handle errors
		// perror("ERROR\nSocket: listen");
	}
}

// connect to client
void Server::connectToClient()
{
	ConnectedClient *newClient = new ConnectedClient(kqueue_fd, fd);
	clients.push_back(newClient);
}
