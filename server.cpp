#include "server.hpp"

// constructor
Server::Server(int const &kqueue_fd) : kqueue_fd(kqueue_fd)
{
	error_pages.insert(std::pair<int,std::string>(404, "./error_pages/404.html"));	//TODO aggiungi altre pagine di errore
	// std::cout << "+ un nuovo server e' stato creato" << std::endl;	//DEBUG
}

// destructor
Server::~Server()
{
	// std::cout << "- un server e' stato distrutto" << std::endl;		//DEBUG
}

// getters
int const	&Server::getKqueueFd() const { return kqueue_fd; }

// communication
void Server::prepareResponse(ConnectedClient &client, void *default_server)
{
	//debug
	Server *ptr = (Server*)default_server;
	ptr = nullptr;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nServer:prepareResponse():\n\nTHE REQUEST FROM FD " << client.connected_fd << " IS: \"" << client.message << "\"" << std::endl;	//DEBUG
	client.message = std::string("SONO UNA RESPONSE\n");	//DEBUG
	//TODO prepare response based on specific server configuration; Request is client.message.

	// add connected_fd to kqueue for WRITE monitoring
	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, client.connected_fd, EVFILT_WRITE, EV_ADD, 0, 0, default_server);		// ident = connected_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)							// filter = WRITE
	{																						// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nServer.prepareResponse: kevent()");
		exit(EXIT_FAILURE);
	}
	std::cout << "\nThe event with ident = " << client.connected_fd << " and filter EVFILT_WRITE has been added to kqueue\n" << std::endl;
}
