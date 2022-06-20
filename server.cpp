#include "server.hpp"

// default constructor
Server::Server(int const &kqueue_fd) : kqueue_fd(kqueue_fd)
{
	error_pages[404] = DEF_404;	//TODO aggiungi altre pagine di errore
}

// copy constructor
Server::Server(Server const &other) : kqueue_fd(other.getKqueueFd()) { *this = other; }

// assign operator overload
Server &Server::operator=(Server const &other)
{
	error_pages = other.error_pages;
	client_body_size = other.client_body_size;
	locations = other.locations;
}

// destructor
Server::~Server() {}

// getters
int const	&Server::getKqueueFd() const { return kqueue_fd; }
std::vector	&Server::getNames() { return names; }

// utility
bool	Server::isName(std::string const &name_to_match) const
{
	for (vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		if (!i->compare(name_to_match))
			return true;
	}
	return false;
}

// ================================================================================================
// communication - prepareResponse - MODIFIED Version, DA TESTARE
// ================================================================================================
void Server::prepareResponse(ConnectedClient &client, const Request & request)
{
	//debug
	// Server *ptr = (Server*)default_server;
	// ptr = nullptr;
	Response response(request);
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nServer:prepareResponse():\n\nTHE REQUEST FROM FD " << client.connected_fd << " IS: \"" << request << "\"" << std::endl;	//DEBUG
	// client.message = std::string("HTTP/1.1 200 OK\r\n\r\n<html><body> <h> SONO UNA RESPONSE </h> </body> </html>");	//DEBUG
	client.message = response.getResponse();	//DEBUG
}

