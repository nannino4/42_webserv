#include "server.hpp"

// default constructor
Server::Server(int const &kqueue_epoll_fd) : kqueue_epoll_fd(kqueue_epoll_fd)
{
	error_pages[404] = DEF_404;	//TODO aggiungi altre pagine di errore
}

// copy constructor
Server::Server(Server const &other) : kqueue_epoll_fd(other.getKqueueFd()) { *this = other; }

// assign operator overload
Server &Server::operator=(Server const &other)
{
	names = other.names;
	error_pages = other.error_pages;
	client_body_size = other.client_body_size;
	locations = other.locations;
	return *this;
}

// destructor
Server::~Server() {}

// getters
int const					&Server::getKqueueEpollFd() const { return kqueue_epoll_fd; }
std::vector<std::string> const	&Server::getNames() const { return names; }

// utility
bool	Server::isName(std::string const &name_to_match) const
{
	for (vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		if (!i->compare(name_to_match))
			return true;
	}
	return false;
}

// operator overload
std::ostream &operator<<(std::ostream &os, Server const &server)
{
	os << "\nServer introducing itself:\n";
	os << "kqueue_epoll_fd:\n" << server.kqueue_epoll_fd << std::endl;
	os << "names:\n";
	for (std::vector<std::string>::const_iterator it = server.names.begin(); it != server.names.end(); ++it)
	{
		os << *it << std::endl;
	}
	os << "error_pages:\n";
	for (std::map<int, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
	{
		os << "code:" << it->first << "\tpath:" << it->second << std::endl;
	}
	os << "client_body_size:\n" << server.client_body_size << std::endl;
	os << "locations:\n";
	for (std::map<std::string,Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it)
	{
		os << it->second << std::endl;
	}
	os << "\nServer introduction is over" << std::endl;
	return os;
}

// ================================================================================================
// communication - prepareResponse - MODIFIED Version, DA TESTARE
// ================================================================================================
void Server::prepareResponse(ConnectedClient *client, const Request &request)
{
	//debug
	// Server *ptr = (Server*)default_server;
	// ptr = nullptr;
	Response response(request);
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nServer:prepareResponse():\n\nTHE REQUEST FROM FD " << client->connected_fd << " IS: \"" << request << "\"" << std::endl;	//DEBUG
	// client->message = std::string("HTTP/1.1 200 OK\r\n\r\n<html><body> <h> SONO UNA RESPONSE </h> </body> </html>");	//DEBUG
	client->message = response.getResponse();	//DEBUG
}

