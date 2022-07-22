#include "server.hpp"

// default constructor
Server::Server(int const &kqueue_epoll_fd) : kqueue_epoll_fd(kqueue_epoll_fd)
{
	error_pages[404] = DEF_404;	//TODO aggiungi altre pagine di errore
}

// copy constructor
Server::Server(Server const &other) : kqueue_epoll_fd(other.getKqueueEpollFd()) { *this = other; }

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
int const								&Server::getKqueueEpollFd() const { return kqueue_epoll_fd; }
std::vector<std::string> const			&Server::getNames() const { return names; }
std::map<int,std::string> const			&Server::getErrorPages() const { return error_pages; }
size_t const							&Server::getClientBodySize() const { return client_body_size; }
std::map<std::string,Location> const	&Server::getLocations() const { return locations; }

// utility
bool	Server::isName(std::string const &name_to_match) const
{
	for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i)
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

// prepareResponse
void Server::prepareResponse(ConnectedClient *client)
{
	Request		&request = client->request;
	Response	&response = client->response;

	if (!request.isValid())
	{
		// the request in not valid
		response.setStatusCode("400");
		response.setReasonPhrase("BAD REQUEST");
	}
	else
	{
		// the request is valid
		if (!request.getLocation()->isMethodAllowed(request.getMethod()))
		{
			// the method requested is not allowed
			response.setStatusCode("400");				//todo check
			response.setReasonPhrase("BAD REQUEST");	//todo check
		}
		else
		{
			// the method requested is allowed
			if (!request.getMethod().compare("GET"))
			{
				//TODO get
			}
			else if (!request.getMethod().compare("POST"))
			{
				//TODO post
			}
			if (!request.getMethod().compare("DELETE"))
			{
				//TODO delete
			}
		}
	}
}

