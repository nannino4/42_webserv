#include "default_server.hpp"

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
int	Server::getKqueueFd() const { return kqueue_fd; }
