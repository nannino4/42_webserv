#include "server.hpp"

// constructor
Server::Server(int const &kqueue_fd) : kqueue_fd(kqueue_fd)
{
	error_pages.insert(std::pair<int,std::string>(404, "./error_pages/404.html"));	//TODO aggiungi altre pagine di errore
	std::cout << "un nuovo server e' stato creato" << std::endl;	//DEBUG
}

// destructor
Server::~Server()
{
	std::cout << "un server e' stato distrutto" << std::endl;		//DEBUG
}

// getters
int const	&Server::getKqueueFd() const { return kqueue_fd; }

// communication
void prepareResponse(ConnectedClient &client)
{
	std::cout << "THE REQUEST FROM FD " << client.getConnectedFd() << " IS:\n" << client.getMessage() << std::endl;	//DEBUG
	client.getMessage() = std::string("ciao, sono una response");	//DEBUG
	//TODO prepare response based on specific server configuration; Request is client.message.

	// add connected_fd to kqueue for WRITE monitoring
	struct kevent event;
	EV_SET(&event, client.getConnectedFd(), EVFILT_WRITE, EV_ADD, 0, 0, (void *)&client);	// ident = connected_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)							// filter = WRITE
	{																						// udata = ConnectedClient*
		//TODO handle error
		perror("ERROR\nDefaultServer.connectToClient: kevent()");
		exit(EXIT_FAILURE);
	}
}
