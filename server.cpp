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

bool	Server::isName(std::string const &name_to_match) const
{
	for (vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		if (!i->compare(name_to_match))
			return true;
	}
	return false;
}

//initialization
// add location
void Server::addLocation(std::string path, Location location)
{
	//TODO
}

// ================================================================================================
// communication - prepareResponse old Version, funzionante
// ================================================================================================
/*
void Server::prepareResponse(ConnectedClient &client, void *default_server)
{
	//debug
	Server *ptr = (Server*)default_server;
	ptr = nullptr;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nServer:prepareResponse():\n\nTHE REQUEST FROM FD " << client.connected_fd << " IS: \"" << client.message << "\"" << std::endl;	//DEBUG
	client.message = std::string("HTTP/1.1 200 OK\r\n\r\n<html><body> <h> SONO UNA RESPONSE </h> </body> </html>");	//DEBUG
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
*/

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

