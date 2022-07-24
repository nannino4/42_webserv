#include "server.hpp"

// default constructor
Server::Server(int const &kqueue_epoll_fd) : kqueue_epoll_fd(kqueue_epoll_fd) {}

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
	Request										&request = client->request;
	Response									&response = client->response;
	std::map<int,std::string>::const_iterator	it;

	if (!request.isValid())
	{
		// the request in not valid
		response.setStatusCode("400");
		response.setReasonPhrase("BAD REQUEST");
		// set body accordingly
		if ((it = error_pages.find(std::atoi(response.getStatusCode().c_str()))) != error_pages.end())
		{
			std::ifstream	error_page_file(it->second);
			std::string		line;

			while (error_page_file.good())
			{
				getline(error_page_file, line);
				response.setBody(response.getBody() + line + "\n");
			}
		}
		else
		{
			response.generateErrorPage();
		}
	}
	else
	{
		// the request is valid
		if (!request.getLocation()->isMethodAllowed(request.getMethod()))
		{
			// the method requested is not allowed
			response.setStatusCode("405");
			response.setReasonPhrase("Method Not Allowed");
			if ((it = error_pages.find(std::atoi(response.getStatusCode().c_str()))) != error_pages.end())
			{
				std::ifstream	error_page_file(it->second);
				std::string		line;

				while (error_page_file.good())
				{
					getline(error_page_file, line);
					response.setBody(response.getBody() + line + "\n");
				}
			}
			else
			{
				response.generateErrorPage();
			}
		}
		else
		{
			// the method requested is allowed
			if (!request.getMethod().compare("GET"))
			{
				methodGet(request, response);
			}
			else if (!request.getMethod().compare("POST"))
			{
				methodPost(request, response);
			}
			if (!request.getMethod().compare("DELETE"))
			{
				methodDelete(request, response);
			}
		}
	}
	response.createResponse();
}

// GET method
void Server::methodGet(Request const &request, Response &response)
{
	// size_t pos = request.getPath().find(".php");
	// if(pos < request.getPath().size())
	// {
	// 	// std::cout << "executing cgi file: " << pos << "\n";
	// 	Cgi cgi(request);
	// 	response.setBody(response.getBody() + cgi.run_cgi("/usr/local/bin/php-cgi"));
	// 	response.setStatusCode("200");
	// 	response.setReasonPhrase("OK");
	// 	response.addNewHeader(std::pair<std::string,std::string>("Content-Length", std::to_string(response.getBody().size())));
	// 	return;
	// }
	std::ifstream		file;
	struct stat			buf;
	std::stringstream	line;

	stat(("." + request.getPath()).c_str(), &buf);
    
    #ifdef __MACH__
        if (S_ISDIR(buf.st_mode))
            manageDir();
        else if (S_ISREG(buf.st_mode))
            getFile(("." + request.getPath()));
    #elif defined(__linux__)
        if ((buf.st_mode & S_IFMT) == S_IFDIR)
            manageDir();
        else if ((buf.st_mode & S_IFMT) == S_IFREG)
            getFile(request, response);
    #endif
	else
	{
		response.setStatusCode("404");
		response.setReasonPhrase("File Not Found");
		response.generateErrorPage();
	}
	//TODO review
}

// POST method
void Server::methodPost(Request const &request, Response &response)
{
	struct stat buf;
    stat(("." + request.getPath()).c_str(), &buf);

    #ifdef __MACH__
        if (S_ISDIR(buf.st_mode))
    #elif defined(__linux__)
    	if ((buf.st_mode & S_IFMT) == S_IFDIR)
    #endif
	{
		response.setStatusCode("500");
		response.setReasonPhrase("Internal Server Error");
		return ;
	}
    #ifdef __MACH__
        else if (S_ISREG(buf.st_mode))
    #elif defined(__linux__)
        else if ((buf.st_mode & S_IFMT) == S_IFREG)
    #endif
	{
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
	}
	else
	{
		response.setStatusCode("201");
		response.setReasonPhrase("Created");
	}
	std::fstream file(("." + request.getPath()).c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
	file << request.getBody();
	file.close();
	//TODO review
}

// DELETE method
void Server::methodDelete(Request const &request, Response &response)
{
	std::ifstream file;
	struct stat buf;

    stat(("." + request.getPath()).c_str(), &buf);

    #ifdef __MACH__
        if (S_ISREG(buf.st_mode))
    #elif defined(__linux__)
        if ((buf.st_mode & S_IFMT) == S_IFREG)
    #endif
	{
		unlink(("." + request.getPath()).c_str());
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
	}
	else
	{
		response.setStatusCode("404");
		response.setReasonPhrase("File Not Found");
		response.generateErrorPage();
	}
	//TODO review
}

// get file to body
void Server::getFile(Request const &request, Response &response)
{
	std::ifstream file;
	std::stringstream line;

	file.open(request.getPath());
	if (file.is_open())
	{
		line << file.rdbuf();
		response.setBody(line.str());
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
	}
	else if (file.fail())
	{
		response.setStatusCode("403");		//TODO check why not 404
		response.setReasonPhrase("Forbidden");
		response.generateErrorPage();
	}
}


void Server::manageDir()
{
	// std::map<std::string,Location>::const_iterator it = srv.getLocations().find(request.getPath());
	// if (it == srv.getLocations().end())
	// {
	// 	if ((it = srv.getLocations().find(request.getPath().substr(1, request.getPath().size()))) == srv.getLocations().end())
	// 	{
	// 		response_status_code = "404";
	// 		reason_phrase = "File Not Found";
	// 		response.generateErrorPage();
	// 		return ;
	// 	}
	// }
	// if (!it->second.getRoot().empty())
	// {
	// 	request.setPath(it->second.getRoot());
	// 	manageDir();
	// }
	// else if (!it->second.getRedirection().first.empty())
	// {
	// 	response_status_code = std::to_string(it->second.getRedirection().second);
	// 	reason_phrase = "Moved Permanently";
	// 	headers["Location"] = it->second.getRedirection().first;
	// 	generateAutoIndex();
	// }
	// else if (it->second.isAutoindex())
	// 	generateAutoIndex();
	// else if (!it->second.getIndex().empty())
	// 	fileTobody("./" + it->first + it->second.getIndex());
	// else
	// {
	// 	response_status_code = "404";
	// 	reason_phrase = "File Not Found";
	// 	response.generateErrorPage();
	// }
	// //TODO review
}
