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

// TODO
// header date
// header allow
// header location
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

		// add the location root path to the request path
		request.setPath(request.getPath() + request.getLocation()->getRoot());

		// check if the method is allowd
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
			// the method request is allowed
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
	std::ifstream								file;
	struct stat									file_stat;
	std::stringstream							line;
	std::map<int,std::string>::const_iterator	it;

	// check for redirection
	if (request.getLocation()->isRedirection())
	{
		response.setStatusCode(std::to_string(request.getLocation()->getRedirection().second));
		response.setReasonPhrase("Moved Permanently");
		response.addNewHeader(std::pair<std::string,std::string>("Location", request.getLocation()->getRedirection().first));
		// generateAutoIndex();	// TODO why?
	}
	else	// no redirection; continue processing request
	{
		// get information about the file identified by path
		if (stat((request.getPath()).c_str(), &file_stat) == 0)
		{
			// check whether path identifies a regular file, or a directory
		    if (S_ISDIR(file_stat.st_mode))			// path identifies a directory
			{
		        manageDir(request, response);
			}
		    else if (S_ISREG(file_stat.st_mode))	// path identifies a regular file
			{
		        getFile(request.getPath(), response);
			}
			else									// path identifies no directory nor file
			{
				response.setStatusCode("404");
				response.setReasonPhrase("File Not Found");
				// set body accordingly
				if ((it = error_pages.find(std::atoi(response.getStatusCode().c_str()))) != error_pages.end())
				{
					std::ifstream	error_page_file(it->second);
					std::string		tmp;

					while (error_page_file.good())
					{
						getline(error_page_file, tmp);
						response.setBody(response.getBody() + tmp + "\n");
					}
				}
				else
				{
					response.generateErrorPage();
				}
			}
		}
		else	// path is invalid
		{
			response.setStatusCode("404");
			response.setReasonPhrase("File Not Found");
			// set body accordingly
			if ((it = error_pages.find(std::atoi(response.getStatusCode().c_str()))) != error_pages.end())
			{
				std::ifstream	error_page_file(it->second);
				std::string		tmp;

				while (error_page_file.good())
				{
					getline(error_page_file, tmp);
					response.setBody(response.getBody() + tmp + "\n");
				}
			}
			else
			{
				response.generateErrorPage();
			}
		}
	}
}

// POST method
void Server::methodPost(Request const &request, Response &response)
{
	struct stat file_stat;
    stat((request.getPath()).c_str(), &file_stat);

	if (S_ISDIR(file_stat.st_mode))
	{
		response.setStatusCode("500");
		response.setReasonPhrase("Internal Server Error");
		return ;
	}
	else if (S_ISREG(file_stat.st_mode))
	{
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
	}
	else
	{
		response.setStatusCode("201");
		response.setReasonPhrase("Created");
	}
	std::fstream file((request.getPath()).c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
	file << request.getBody();
	file.close();
	//TODO review
}

// DELETE method
void Server::methodDelete(Request const &request, Response &response)
{
	std::ifstream								file;
	struct stat									file_stat;
	std::map<int,std::string>::const_iterator	it;

    if (stat((request.getPath()).c_str(), &file_stat) == 0)
	{
		// the path is valid
		if (S_ISREG(file_stat.st_mode))
		{
			// the path identifies a regular file
			unlink((request.getPath()).c_str());
			response.setStatusCode("200");
			response.setReasonPhrase("OK");
		}
		else
		{
			// the path does not identify a regular file
			response.setStatusCode("404");
			response.setReasonPhrase("File Not Found");
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
	}
	else
	{
		// the path is invalid
		response.setStatusCode("404");
		response.setReasonPhrase("File Not Found");
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
}

// get file to body
void Server::getFile(std::string const path, Response &response) 
{
	std::ifstream								file;
	std::stringstream							line;
	std::map<int,std::string>::const_iterator	it;

	file.open(path);
	if (file.is_open())
	{
		line << file.rdbuf();
		response.setBody(line.str());
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
	}
	else if (file.fail())
	{
		response.setStatusCode("403");
		response.setReasonPhrase("Forbidden");
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
}

// manage case in which path identifies a directory
void Server::manageDir(Request const &request, Response &response)
{
	std::map<int,std::string>::const_iterator	it;
	struct stat									file_stat;

	if (!request.getLocation()->getIndex().empty() && \
			stat(std::string(request.getPath() + request.getLocation()->getIndex()).c_str(), &file_stat) == 0 && \
			S_ISREG(file_stat.st_mode))
	{
		// an index exists and if it is a file
		getFile(request.getPath() + request.getLocation()->getIndex(), response);
	}
	else if (request.getLocation()->isAutoindex())
	{
		// no indexes, but autoindex is on
		generateAutoIndex();
	}
	else
	{
		// no indexes, no autoindex
		response.setStatusCode("404");
		response.setReasonPhrase("File Not Found");
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
}

// generate autoindex
void Server::generateAutoIndex()
{
// 	DIR *dir;
// 	struct dirent *ent;
// 	std::stringstream line;

// 	if ((dir = opendir(("./" + request.getPath()).c_str())) != NULL)
// 	{
// 		line << "<html><head><title>Index of " << request.getPath() << reason_phrase << "</title>";
// 		line << "<link rel=\"stylesheet\" href=\"/pages/base.css\"";
// 		line << "<meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
// 		line << "</head><body><main>";
		
// 		line << "<h1>Index of " << request.getPath() << "</h1>\n";
// 		line << "<table>";
// 		/* print all the files and directories within directory */
// 		while ((ent = readdir(dir)) != NULL)
// 		{
// 			if (std::string(ent->d_name) != ".")
// 			{
// 				line << "<tr><td><a href=\"" << ent->d_name << "\">";
// 				line << ent->d_name << "</a></td></tr>";
// 			}
// 		}
// 		line << "</table></main></body></html>";
// 		body = line.str();
// 		closedir (dir);
// 	}
// 	else
// 	{
// 		/* could not open directory */
// 		perror ("could not open directory");
// 		response_status_code = "404";
// 		reason_phrase = "File Not Found";
// 		response.generateErrorPage();
// 	}
// 	//TODO review
}
