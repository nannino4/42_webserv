#include "default_server.hpp"

// default constructor
DefaultServer::DefaultServer(int const &kqueue_epoll_fd, unsigned int backlog, std::string &config_file, int &pos) : Server(kqueue_epoll_fd), backlog(backlog), listening_fd(-1), triggered_event(this->listening_fd, this, this)
{
	// default initialization
	bzero(buf, BUFFER_SIZE);
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_addr.s_addr = DEF_ADDR;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(DEF_PORT);

	// specific initialization

	std::stringstream	stream;
	int					found_pos;

	// check if file doesn't contain any character among "{;}" - '}' is required to close the "server" block
	if ((unsigned long)(found_pos = config_file.find_first_of("{;}", pos, 3)) == std::string::npos)
	{
		std::cerr << "\nERROR\nDefaultServer::DefaultServer(): expected '}'" << std::endl;
		exit(EXIT_FAILURE);
	}

	// parse server block searching for directives and 'location' blocks
	while (config_file[found_pos] != '}')
	{
		stream.clear();
		stream.str(config_file.substr(pos, (found_pos - pos)));
		pos = found_pos + 1;
	
		if (config_file[found_pos] == ';')
		{
			parseDirectives(stream);
		}
		else if (config_file[found_pos] == '{')
		{
			std::string	directive;
			std::string	path;

			stream >> directive >> path;
			if (!stream.eof())
				stream >> std::ws;


			// check that stream didn't fail reading
			if (stream.fail())
			{
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): stream reading failed" << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that directive == "location"
			if (directive.compare("location"))
			{
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): directive \"" << directive << "\" is not valid. Expected \"location\"." << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that path exists
			if (path.empty())
			{
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): the directive \"location\" should be followed by [path]" << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that stream reached EOF
			if (!stream.eof())
			{
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): too many parameters parsing \"[location] [path] {\"" << std::endl;
				exit(EXIT_FAILURE);
			}

			// fix the path format regarding the '/' char (/valid/path)
			if (path.empty() || path.at(0) != '/')
				path.insert(path.begin(), '/');
			if (path != "/" && path.back() == '/')
				path.erase(path.end() - 1);

			// check that insert() actually inserted a new element
			if (!(locations.insert(std::pair<std::string,Location>(path, Location(config_file, pos)))).second)
			{
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): could not add location: a location with path \"" << path << "\" already exists" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if ((unsigned long)(found_pos = config_file.find_first_of("{;}", pos, 3)) == std::string::npos)
		{
			std::cerr << "\nERROR\nDefaultServer::DefaultServer(): expected '}'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// set pos after found_pos - which is pointing to '}'
	pos = found_pos + 1;
}

// copy constructor
DefaultServer::DefaultServer(DefaultServer const &other) : Server(other), triggered_event(other.triggered_event) { *this = other; }

// assign operator
DefaultServer &DefaultServer::operator=(DefaultServer const &other)
{
	backlog = other.backlog;
	server_addr = other.server_addr;
	virtual_servers = other.virtual_servers;
	clients = other.clients;
	listening_fd = other.getListeningFd();
	bzero(buf, BUFFER_SIZE);
	return *this;
}

// destructor
DefaultServer::~DefaultServer()
{
	if (listening_fd != -1)
		close(listening_fd);
	for (std::map<int,ConnectedClient&>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		it->second.~ConnectedClient();
		delete &(*it);
	}
}

// operator overload
std::ostream &operator<<(std::ostream &os, DefaultServer const &def_serv)
{
	os << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	os << "\n\tDefaultServer introducing itself:\n";
	os << "\tdefault server names:\t";
	for (std::vector<std::string>::const_iterator name = def_serv.getNames().begin(); name != def_serv.getNames().end(); ++name)
	{
		os << " " << *name;
	}
	os << std::endl;
	os << "\tserver_addr:\t\t" << inet_ntoa(def_serv.server_addr.sin_addr) << ":" << ntohs(def_serv.server_addr.sin_port) << std::endl;
	os << "\tlistening_fd:\t\t" << def_serv.listening_fd << std::endl;
	os << "\tlocations:\t\t" << def_serv.locations.size() << std::endl;
	// for (std::map<std::string,Location>::const_iterator it = def_serv.locations.begin(); it != def_serv.locations.end(); ++it)
	// {
	// 	os << it->second << std::endl;
	// }
	os << "\tvirtual_servers:\t" << def_serv.virtual_servers.size() << std::endl;
	for (std::vector<Server>::const_iterator it = def_serv.virtual_servers.begin(); it != def_serv.virtual_servers.end(); ++it)
	{
		os << "\t\tvirtual server names:\t";
		for (std::vector<std::string>::const_iterator name = it->getNames().begin(); name != it->getNames().end(); ++name)
		{
			os << " " << *name;
		}
		os << std::endl;
	}
	os << "\tclients:\t\t" << def_serv.clients.size() << std::endl;
	for (std::map<int,ConnectedClient&>::const_iterator it = def_serv.clients.begin(); it != def_serv.clients.end(); ++it)
	{
		os << "\t\tconnected_fd:\t\t" << it->second.connected_fd << std::endl;
		os << "\t\tRequest:\t\"" << it->second.request.getRequest() << "\"" << std::endl;
	}
	os << "\tDefaultServer introduction is over" << std::endl;
	os << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	return os;
}

// getters
DefaultServer::address	DefaultServer::getAddress() const { return address(server_addr.sin_addr.s_addr, server_addr.sin_port); }
std::string				DefaultServer::getIp() const { return inet_ntoa(server_addr.sin_addr); }
std::string				DefaultServer::getPort() const { return std::to_string(ntohs(server_addr.sin_port)); }
unsigned int const		&DefaultServer::getBacklog() const { return backlog; }
int const				&DefaultServer::getListeningFd() const { return listening_fd; }

// initialization
// add virtual server
void DefaultServer::addVirtualServer(DefaultServer &new_serv)
{
	std::vector<std::string>	tmp_names = new_serv.getNames();

	for (std::vector<std::string>::const_iterator name_to_match = tmp_names.begin(); name_to_match != tmp_names.end(); ++name_to_match)
	{
		for (std::vector<Server>::const_iterator i = virtual_servers.begin(); i != virtual_servers.end(); ++i)
		{
			if (i->isName(*name_to_match))
			{
				std::cerr << "\nERROR\nDefaultServer::addVirtualServer(): cannot add new virtual server: name already used" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	virtual_servers.push_back((Server)new_serv);
}

// communication
void DefaultServer::startListening()
{
	int optval = true;		// used in setsockopt()

	listening_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_fd < 0)
	{
		perror("ERROR\nDefaultServer.startListening(): socket()");
		exit(EXIT_FAILURE);
	}
	setsockopt(listening_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));	//useful to restart immediately the server, without having to wait because of TCP security
	if (bind(listening_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("ERROR\nDefaultServer.startListening(): bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(listening_fd, backlog) == -1)
	{
		perror("ERROR\nDefaultServer.startListening(): listen()");
		exit(EXIT_FAILURE);
	}

	triggered_event.events = READ;
#ifdef __MACH__
	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, listening_fd, EVFILT_READ, EV_ADD, 0, 0, &triggered_event);
	if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
#elif defined(__linux__)
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = EPOLLIN;
	event.data.ptr = &triggered_event;
	if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, listening_fd, &event) == -1)
#endif
	{
		perror("ERROR\nDefaultServer.startListening(): kevent()/epoll_ctl()");
		exit(EXIT_FAILURE);
	}

	//DEBUG
	// // std::cout << "-----------------------------------------------------------" << std::endl;
	// // std::cout << "-----------------------------------------------------------" << std::endl;
	// // std::cout << "\nDefaultServer:startListening:\n\nlistening on fd = " << listening_fd << 
			// "\nport = " << ntohs(server_addr.sin_port) << 
			// "\nip = " << inet_ntoa(server_addr.sin_addr) << std::endl << std::endl;
}

void DefaultServer::connectToClient()
{
	struct sockaddr_in	client_addr;
	int					connected_fd;

	bzero(&client_addr, sizeof(client_addr));
	socklen_t socklen = sizeof(client_addr);
	client_addr.sin_family = AF_INET;
	connected_fd = accept(listening_fd, (sockaddr *)&client_addr, &socklen);
	if (connected_fd == -1)
	{
		//accept() failed
		perror("ERROR\nDefaultServer.connectToClient(): accept()");
	}
	else
	{
		// accept() was succesful

		//DEBUG
		std::cout << "-----------------------------------------------------------" << std::endl;
		std::cout << "-----------------------------------------------------------" << std::endl;
		std::cout << "\nDefaultServer:connectToClient()\n\nconnected to fd = " << connected_fd << "\nport = " << ntohs(client_addr.sin_port) << "\nip = " << inet_ntoa(client_addr.sin_addr) << std::endl << std::endl;

		// create new ConnectedClient
		ConnectedClient *new_client = new ConnectedClient(connected_fd, client_addr, this);
		clients.insert(std::pair<int,ConnectedClient&>(connected_fd, *new_client));

		// add new connected_fd to kqueue for READ monitoring
		new_client->triggered_event.events = READ;
	#ifdef __MACH__
		struct kevent event;
		bzero(&event, sizeof(event));
		EV_SET(&event, connected_fd, EVFILT_READ, EV_ADD, 0, 0, &new_client->triggered_event);
		if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
	#elif defined(__linux__)
		struct epoll_event event;
		bzero(&event, sizeof(event));
		event.events = EPOLLIN | EPOLLRDHUP;
		event.data.ptr = &new_client->triggered_event;
		if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, connected_fd, &event) == -1)
	#endif
		{
			perror("ERROR\nDefaultServer.connectToClient(): kevent()/epoll_ctl()");
			disconnectFromClient(new_client);
		}
	}
}

void DefaultServer::disconnectFromClient(ConnectedClient *client)
{
	if (clients.erase(client->connected_fd))
		delete client;
}

void DefaultServer::receiveRequest(Event *current_event)
{
	int 				connected_fd = current_event->fd;
	int 				found_pos;
	std::stringstream	stream;
	std::string			tmp;

	//debug
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer.receiveRequest():" << std::endl;

	ConnectedClient *client = (ConnectedClient *)current_event->owner;

	// read from connected_fd into client->request
	int read_bytes = recv(connected_fd, buf, BUFFER_SIZE, 0);
	if (read_bytes == -1 || read_bytes == 0)
	{
		perror("ERROR\nDefaultServer.receiveRequest(): recv");
		disconnectFromClient(client);
		return ;
	}

	// update request
	tmp = client->request.getRequest();
	for (int i = 0; i < read_bytes; ++i)
	{
		tmp.push_back(buf[i]);
	}

	//debug
	// // std::cout << "previous request size = " << client->request.getRequest().size() << std::endl;
	// // std::cout << "read bytes = " << read_bytes << std::endl;
	// // std::cout << "expected request size = " << client->request.getRequest().size() + read_bytes << std::endl;

	client->request.setRequest(tmp);
	bzero(buf, BUFFER_SIZE);

	//debug
	// // std::cout << "current request size = " << client->request.getRequest().size() << std::endl << std::endl;

	// parse newly received request lines
	while (((unsigned long)(found_pos = client->request.getRequest().find("\n", client->request.getRequestPos())) != std::string::npos) \
			&& !client->request.isComplete())
	{
		stream.clear();
		stream.str(client->request.getRequest().substr(client->request.getRequestPos(), (found_pos + 1 - client->request.getRequestPos())));
		client->request.setRequestPos(found_pos + 1);

		if (client->request.getMethod().empty())		// first line
		{
			std::string method;
			std::string path;
			std::string version;

			stream >> method >> path >> version;
			if (!stream.eof())
				stream >> std::ws;

			// check for query
			if (path.find('?') != std::string::npos)
			{
				client->request.setQuery(path.substr(path.find('?') + 1));
				path.erase(path.find('?'));
			}
			else if (path.find('#') != std::string::npos)
			{
				client->request.setQuery(path.substr(path.find('#') + 1));
				path.erase(path.find('#'));
			}

			// fix the path format /valid/path/format
			if (path.empty() || path.at(0) != '/')
				path.insert(path.begin(), '/');
			if (path != "/" && path.back() == '/')
				path.erase(path.end() - 1);

			client->request.setMethod(method);
			client->request.setPath(path);
			client->request.setVersion(version);

			// check that stream didn't fail reading && stream reached EOF && version is correct (HTTP/1.1)
			if (stream.fail() || !stream.eof())
			{
				// the request will stop being handled
				client->request.setIsComplete(true);
				client->request.setIsValid(false);
				client->response.setStatusCode("400");
				client->response.setReasonPhrase("Bad Request");
			}
			else if (method != "GET" && method != "POST" && method != "DELETE")
			{
				// the method is unknown
				client->request.setIsComplete(true);
				client->request.setIsValid(false);
				client->response.setStatusCode("501");
				client->response.setReasonPhrase("Not Implemented");
			}
			else if (client->request.getVersion().compare("HTTP/1.1"))
			{
				// the request will stop being handled
				client->request.setIsComplete(true);
				client->request.setIsValid(false);
				client->response.setStatusCode("505");
				client->response.setReasonPhrase("HTTP Version Not Supported");
			}
		}
		else if (client->request.areHeadersComplete())
		{
			// received body
			std::map<std::string,std::string>::const_iterator	it = client->request.getHeaders().find("Content-Length");
			long unsigned int									content_length;

			// check that header "Content-Lenght" exists
			if (it == client->request.getHeaders().end())
			{
				client->request.setIsComplete(true);
				client->request.setIsValid(false);
				client->response.setStatusCode("411");
				client->response.setReasonPhrase("Length Required");
			}
			else
			{
				content_length = std::atoi(it->second.c_str());
				client->request.setBody(client->request.getBody() + stream.str());

				// check if body is too large
				if ((client_body_size > 0) && (client->request.getBody().size() > client_body_size))
				{
					// the request will stop being handled
					client->request.setIsComplete(true);
					client->request.setIsValid(false);
					client->response.setStatusCode("413");
					client->response.setReasonPhrase("Content Too Large");
				}
				else if (client->request.getBody().size() >= content_length)
				{
					client->request.setIsComplete(true);
				}
			}
		}
		else											// header line || empty line
		{
			if (!stream.str().compare("\r\n") || !stream.str().compare("\n"))
			{
				std::map<std::string,std::string>::const_iterator	it = client->request.getHeaders().find("Content-Length");

				// headers are complete
				client->request.setAreHeadersComplete(true);
				// if the header "Host" is missing the request is invalid
				if (client->request.getHeaders().find("Host") == client->request.getHeaders().end())
				{
					client->request.setIsComplete(true);
					client->request.setIsValid(false);
					client->response.setStatusCode("400");
					client->response.setReasonPhrase("Bad Request");
				}
				// if the method is not POST the request is complete
				if (client->request.getMethod().compare("POST"))
				{
					client->request.setIsComplete(true);
				}
				else if (it == client->request.getHeaders().end())	// check that header "Content-Lenght" exists
				{
					client->request.setIsComplete(true);
					client->request.setIsValid(false);
					client->response.setStatusCode("411");
					client->response.setReasonPhrase("Length Required");
				}
				else if (it->second == "0")
				{
					client->request.setIsComplete(true);
				}
			}
			else if (stream.str().find(":") != std::string::npos)
			{
				// parse header
				std::string			key;
				std::string			value;
				std::string			tmp_string;
				std::stringstream	tmp_ss;

				std::getline(stream, key, ':');
				tmp_ss.str(key);
				tmp_ss >> key;
				tmp_ss.clear();

				std::getline(stream, value);
				tmp_ss.str(value);
				tmp_ss >> value;
				tmp_ss >> std::ws;
				while (tmp_ss.good())
				{
					tmp_ss >> tmp_string >> std::ws;
					value += tmp_string;
				}

				client->request.addHeader(std::pair<std::string,std::string>(key, value));
			}
			else
			{
				// header is invalid and is ignored
			}
		}

		//check if the request is too long
		if (client->request.getMethod() != "POST" && client->request.getRequest().size() > REQUEST_SIZE_LIMIT)
		{
			// remove connected_fd from kqueue
		#ifdef __MACH__
			struct kevent event;
			bzero(&event, sizeof(event));
			EV_SET(&event, client->connected_fd, EVFILT_READ, EV_DELETE, 0, 0, &client->triggered_event);
			if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
		#elif defined(__linux__)
			if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, client->connected_fd, nullptr) == -1)
		#endif
			{
				perror("\nERROR\nDefaultServer.receiveRequest(): kevent()/epoll_ctl()");
			}
			// erase client from the map of clients and delete it from memory
			clients.erase(client->connected_fd);
			delete client;
			return ;
		}
	}

	// update client timeout
	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &client->time_since_last_action);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &client->time_since_last_action);
	#endif

	// debug
	// // std::cout << "\nrequest.body.size =\t" << client->request.getBody().size() << std::endl;
	// if (client->request.getHeaders().find("Content-Length") != client->request.getHeaders().end())
	// 	// std::cout << "Content-Length =\t" << client->request.getHeaders().find("Content-Length")->second << std::endl;

	if (client->request.isComplete())
	{
		// remove connected_fd to kqueue from READ monitoring
	#ifdef __MACH__
		struct kevent event;
		bzero(&event, sizeof(event));
		EV_SET(&event, connected_fd, EVFILT_READ, EV_DELETE, 0, 0, current_event);
		if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
	#elif defined(__linux__)
		if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, connected_fd, nullptr) == -1)
	#endif
		{
			perror("ERROR\nDefaultServer.receiveRequest: kevent()/epoll_ctl()");
		}

		// debug
		std::cout << "---------request without body:---------\n" << client->request.getRequest().substr(0, client->request.getRequest().find("\r\n\r\n")) << std::endl << std::endl;
		dispatchRequest(client);

		// debug
		std::cout << "\nEND of DefaultServer.receiveRequest() terminated" << std::endl;
		std::cout << "-----------------------------------------------------------" << std::endl;
	}
}

void DefaultServer::dispatchRequest(ConnectedClient *client)
{
	//debug
	// std::cout << "\n-------------------------------" << std::endl;
	// std::cout << "DefaultServer::dispatchRequest()\n" << std::endl;

	// find the server corresponding to the host header value
	Server *requestedServer = this;
	if (client->request.isValid())
	{
		for (std::vector<Server>::iterator it = virtual_servers.begin(); it != virtual_servers.end(); ++it)
		{
			if (it->isName(client->request.getHostname().substr(
				0, client->request.getHostname().find(":")
			)))
				requestedServer = &(*it);
		}
	}

	// find the server location correspongin to the request path
	client->request.setLocation(&default_location);
	std::string	path = client->request.getPath();
	do
	{
		if (requestedServer->getLocations().find(path) != requestedServer->getLocations().end())
		{
			client->request.setLocation(&requestedServer->getLocations().find(path)->second);
		}
		path = path.substr(0, path.find_last_of('/'));
	} while ((path.find_last_of('/') != std::string::npos) && (client->request.getLocation() == &default_location));

	// let the server prepare the response
	requestedServer->prepareResponse(client);

	// add connected_fd to kqueue for WRITE monitoring
	client->triggered_event.events = WRITE;
#ifdef __MACH__
	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, client->connected_fd, EVFILT_WRITE, EV_ADD, 0, 0, &client->triggered_event);
	if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
#elif defined(__linux__)
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = EPOLLOUT;
	event.data.ptr = &client->triggered_event;
	if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, client->connected_fd, &event) == -1)
#endif
	{
		perror("ERROR\nDefaultServer.dispatchRequest: kevent()/epoll_ctl()");
		disconnectFromClient(client);
		return ;
	}

	// update client timeout

	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &client->time_since_last_action);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &client->time_since_last_action);
	#endif

	// std::cout << "END of DefaultServer::dispatchRequest()" << std::endl;
	// std::cout << "-------------------------------" << std::endl;
}

void DefaultServer::sendResponse(Event *current_event)
{
	int connected_fd = current_event->fd;
	ConnectedClient *client = (ConnectedClient *)current_event->owner;

	//DEBUG
	std::cout << "\n-----------------------------------------------------------" << std::endl;
	std::cout << "DefaultServer:sendResponse():\n\n" << std::endl;
	
	int buf_siz = ((unsigned long)(client->response.getResponsePos() + BUFFER_SIZE) > client->response.getResponse().size()) ? (client->response.getResponse().size() - client->response.getResponsePos()) : BUFFER_SIZE;

	//debug
	// std::cout << "sono prima del send" << std::endl;

	int sent_bytes = send(connected_fd, client->response.getResponse().substr(client->response.getResponsePos()).c_str(), buf_siz, 0);

	//debug
	std::cout << "sent bytes = " << sent_bytes << std::endl;

	// check that send() didn't fail
	if (sent_bytes == -1 || sent_bytes == 0)
	{
		std::cerr << "ERROR\nDefaultServer.sendResponse(): send()" << std::endl;
		disconnectFromClient(client);
		return ;
	}

	client->response.setResponsePos(client->response.getResponsePos() + sent_bytes);

	// //debug
	// // std::cout << "response sent by now:\n" << client->response.getResponse().substr(0, client->response.getResponsePos()) << std::endl;

	// update client timeout
	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &client->time_since_last_action);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &client->time_since_last_action);
	#endif

	// check if whole response has been sent
	if ((size_t)client->response.getResponsePos() == client->response.getResponse().size() || current_event->is_error || current_event->is_hang_up)
	{

		//DEBUG
		std::cout << "\nThe whole response has been sent" << std::endl;

		// remove connected_fd from kqueue
	#ifdef __MACH__
		struct kevent event;
		bzero(&event, sizeof(event));
		EV_SET(&event, connected_fd, EVFILT_WRITE, EV_DELETE, 0, 0, current_event);
		if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
	#elif defined(__linux__)
		if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, connected_fd, nullptr) == -1)
	#endif
		{
			perror("\nERROR\nDefaultServer.sendResponse(): kevent()/epoll_ctl()");
		}

		// erase client from the map of clients and delete it from memory
		clients.erase(connected_fd);
		delete client;
	}

	// //debug
	// struct epoll_event event;
	// bzero(&event, sizeof(event));
	// event.events = EPOLLIN | EPOLLRDHUP;
	// if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, listening_fd, &event) == -1)
	// 	perror("trying to add listening fd again to epoll");

	//DEBUG
	std::cout << "\nEND of DefaultServer:sendResponse():" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;
}

void DefaultServer::closeTimedOutConnections()
{

	//DEBUG
	// std::cout << "\n-----------------------------------------------------------" << std::endl;
	// std::cout << "DefaultServer:closeTimedOutConnections():" << std::endl;

	ConnectedClient	*current_client;
	std::map<int,ConnectedClient&>::iterator it = clients.begin();

	while (it != clients.end())
	{
		current_client = &it->second;
		++it;

		// check if the connection is timed out
		if (getTimeDifference(current_client->time_since_last_action) > TIMEOUT)
		{
			if (current_client->triggered_event.events == WRITE)	// client timed out reading the response
			{
				// remove connected_fd from kqueue
			#ifdef __MACH__
				struct kevent event;
				bzero(&event, sizeof(event));
				EV_SET(&event, current_client->connected_fd, EVFILT_WRITE, EV_DELETE, 0, 0, &current_client->triggered_event);
				if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
			#elif defined(__linux__)
				if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, current_client->connected_fd, nullptr) == -1)
			#endif
				{
					perror("\nERROR\nDefaultServer.closeTimedOutConnections(): kevent()/epoll_ctl()");
				}

				//debug
				// std::cout << "removing conneted_fd " << current_client->connected_fd << " because it TIMED OUT reading the response" << std::endl;

				// erase client from the map of clients and delete it from memory
				clients.erase(current_client->connected_fd);
				delete current_client;
			}
			else if (current_client->request.getRequest().empty())		// client didn't send any request and the connection timed out
			{
				// remove connected_fd from kqueue
			#ifdef __MACH__
				struct kevent event;
				bzero(&event, sizeof(event));
				EV_SET(&event, current_client->connected_fd, EVFILT_READ, EV_DELETE, 0, 0, &current_client->triggered_event);
				if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
			#elif defined(__linux__)
				if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, current_client->connected_fd, nullptr) == -1)
			#endif
				{
					perror("\nERROR\nDefaultServer.closeTimedOutConnections(): kevent()/epoll_ctl()");
				}

				//debug
				// std::cout << "removing conneted_fd " << current_client->connected_fd << " because it TIMED OUT" << std::endl;

				// erase client from the map of clients and delete it from memory
				current_client->~ConnectedClient();
				clients.erase(current_client->connected_fd);
				delete current_client;
			}
			else														// client timed out sending the request
			{
				// the request is considered complete, even though invalid

				current_client->request.setIsValid(false);
				current_client->response.setStatusCode("408");
				current_client->response.setReasonPhrase("Request Timeout");
				
				// remove connected_fd to kqueue from READ monitoring
			#ifdef __MACH__
				struct kevent event;
				bzero(&event, sizeof(event));
				EV_SET(&event, current_client->connected_fd, EVFILT_READ, EV_DELETE, 0, 0, &current_client->triggered_event);
				if (kevent(kqueue_epoll_fd, &event, 1, nullptr, 0, nullptr) == -1)
			#elif defined(__linux__)
				if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_DEL, current_client->connected_fd, nullptr) == -1)
			#endif
				{
					perror("ERROR\nDefaultServer.closeTimedOutConnections: kevent()/epoll_ctl()");
				}

				// dispatch request as if it was complete, yet invalid
				dispatchRequest(current_client);
			}
		}
	} // while (it != clients.end())
	
	//DEBUG
	// std::cout << "\nEND of DefaultServer:closeTimedOutConnections():" << std::endl;
	// std::cout << "-----------------------------------------------------------" << std::endl;
}
