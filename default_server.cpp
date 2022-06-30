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
	error_pages[404] = std::string(DEF_404);	//TODO aggiungi altre pagine di errore

	// specific initialization

	std::stringstream	stream;
	int					found_pos;

	// check if file doesn't contain any character among "{;}" - '}' is required to close the "server" block
	if ((unsigned long)(found_pos = config_file.find_first_of("{;}", pos, 3)) == std::string::npos)
	{
		//TODO handle error
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

			// check that stream didn't fail reading
			if (stream.fail())
			{
				//TODO handle error
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): stream reading failed" << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that directive == "location"
			if (directive.compare("location"))
			{
				//TODO handle error
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): directive \"" << directive << "\" is not valid. Expected \"location\"." << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that path exists
			if (path.empty())
			{
				//TODO handle error
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): the directive \"location\" should be followed by [path]" << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that stream reached EOF
			if (!stream.eof())
			{
				//TODO handle error
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): too many parameters parsing \"[location] [path] {\"" << std::endl;
				exit(EXIT_FAILURE);
			}

			// check that insert() actually inserted a new element
			if (!(locations.insert(std::pair<std::string,Location>(path, Location(config_file, pos)))).second)
			{
				//TODO handle error
				std::cerr << "\nERROR\nDefaultServer::DefaultServer(): could not add location: a location with path \"" << path << "\" already exists" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if ((unsigned long)(found_pos = config_file.find_first_of("{;}", pos, 3)) == std::string::npos)
		{
			//TODO handle error
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
	os << "\n\tDefaultServer introducing itself:\n";
	os << "\tserver_addr:\t\t" << inet_ntoa(def_serv.server_addr.sin_addr) << ":" << ntohs(def_serv.server_addr.sin_port) << std::endl;
	os << "\tlistening_fd:\t\t" << def_serv.listening_fd << std::endl;
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
		os << "\t\tmessage:\t\"" << it->second.message << "\"" << std::endl;
	}
	os << "\tDefaultServer introduction is over" << std::endl;
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
				//TODO handle error
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
	int optval = true;

	listening_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_fd < 0)
	{
		//TODO handle errors
		perror("ERROR\nDefaultServer.startListening(): socket()");
		exit(EXIT_FAILURE);
	}
	setsockopt(listening_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));	//useful to restart immediately the server, without having to wait because of TCP security
	if (bind(listening_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		//TODO handle errors
		perror("ERROR\nDefaultServer.startListening(): bind()");
		exit(EXIT_FAILURE);
	}
	if (listen(listening_fd, backlog) == -1)
	{
		//TODO handle errors
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
		//TODO handle error
		perror("ERROR\nDefaultServer.startListening(): kevent()/epoll_ctl()");
		exit(EXIT_FAILURE);
	}

	//DEBUG
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer:startListening:\n\nlistening on fd = " << listening_fd << \
			"\nport = " << ntohs(server_addr.sin_port) << \
			"\nip = " << inet_ntoa(server_addr.sin_addr) << std::endl << std::endl;
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
		//TODO handle error
		perror("ERROR\nDefaultServer.connectToClient(): accept()");
		exit(EXIT_FAILURE);
	}

	//DEBUG
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer:connectToClient()\n\nconnected to fd = " << connected_fd << \
			"\nport = " << ntohs(client_addr.sin_port) << \
			"\nip = " << inet_ntoa(client_addr.sin_addr) << std::endl << std::endl;

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
	event.events = EPOLLIN;
	event.data.ptr = &new_client->triggered_event;
	if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, connected_fd, &event) == -1)
#endif
	{
		//TODO handle error
		perror("ERROR\nDefaultServer.connectToClient(): kevent()/epoll_ctl()");
		exit(EXIT_FAILURE);
	}

	//debug
	std::cout << "\nThe event with ident = " << connected_fd << " and filter EVFILT_READ has been added to kqueue" << \
			"\nThere are currently " << clients.size() << " clients connected to this address:port\n" << std::endl;
}

void DefaultServer::receiveRequest(Event *current_event)
{
	int connected_fd = current_event->fd;

	//debug
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer.receiveRequest():" << std::endl;

	ConnectedClient *client = (ConnectedClient *)current_event->owner;

	// read from connected_fd into client->message

	//debug
	std::cout << "\ngoing to try recv" << std::endl;

	int read_bytes = recv(connected_fd, buf, BUFFER_SIZE - 1, 0);
	if (read_bytes == -1)
	{
		//TODO handle error
		
		// perror("ERROR\nDefaultServer.receiveRequest(): recv");
		exit(EXIT_FAILURE);
	}
	client->message += buf;

	//debug
	std::cout << "\nread_bytes = " << read_bytes << "\ntotal message = \n\"" << client->message << "\"" << std::endl;

	// debug
	/* std::cout << "\nreceived data = \"" << buf << "\"" \
 		"\nreceived request = \"" << client->message << "\"" \
 		"\nread_bytes = " << read_bytes << \
		"\nBUFFER_SIZE - 1 = " << BUFFER_SIZE - 1 << \
		"\nEOF = " << (event.flags & EV_EOF) << std::endl;
	*/
	bzero(buf, BUFFER_SIZE);

	if ((read_bytes < (BUFFER_SIZE - 1)))
	{
		//DEBUG
		std::cout << "\nThe whole request has been received" << std::endl;

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
			//TODO handle error
			perror("ERROR\nDefaultServer.receiveRequest: kevent()/epoll_ctl()");
			exit(EXIT_FAILURE);
		}

		//debug
		std::cout << "\nThe event with ident = " << client->connected_fd << " and filter EVFILT_READ has been removed from kqueue\n" << std::endl;
		dispatchRequest(client);
	}
}

void DefaultServer::dispatchRequest(ConnectedClient *client)
{
	//TODO dispatch the request to the corresponding server, based on the 'host' value
	Request request(client->message);
	
	Server *serverRequested = this;
	for (std::vector<Server>::iterator it = virtual_servers.begin(); it != virtual_servers.end(); ++it)
	{
		if (it->isName(request.getHostname()))
			serverRequested = &(*it);
	}
	
	serverRequested->prepareResponse(client, request);
	
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
	event.events = EPOLLIN;
	event.data.ptr = &client->triggered_event;
	if (epoll_ctl(kqueue_epoll_fd, EPOLL_CTL_ADD, client->connected_fd, &event) == -1)
#endif
	{
		//TODO handle error
		perror("ERROR\nDefaultServer.dispatchRequest: kevent()/epoll_ctl()");
		exit(EXIT_FAILURE);
	}

	//debug
	std::cout << "\nThe event with ident = " << client->connected_fd << " and filter EVFILT_WRITE has been added to kqueue\n" << std::endl;
}

void DefaultServer::sendResponse(Event *current_event)
{
	int connected_fd = current_event->fd;
	ConnectedClient *client = (ConnectedClient *)current_event->owner;

	//DEBUG
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer:sendResponse():\n\nTHE RESPONSE TO FD " << connected_fd << " IS: \"" << client->message << "\"" << std::endl;	//DEBUG
	
	int buf_siz = ((unsigned long)(client->message_pos + BUFFER_SIZE) > client->message.size()) ? (client->message.size() - client->message_pos) : BUFFER_SIZE;
	int sent_bytes = send(connected_fd, client->message.substr(client->message_pos).c_str(), buf_siz, 0);

	// check that send() didn't fail
	if (sent_bytes == -1)
	{
		//TODO handle error
		std::cerr << "ERROR\nDefaultServer.sendResponse(): send()" << std::endl;
		exit(EXIT_FAILURE);
	}

	client->message_pos += sent_bytes;

	//debug
	std::cout << "\nsent_bytes = " << sent_bytes << "\ntotal message sent = \n\"" << client->message.substr(0, client->message_pos) << "\"" << std::endl;

	if ((size_t)client->message_pos == client->message.size() || current_event->is_error || current_event->is_hang_up)
	{
		//The whole response has been sent

		//debug
		std::cout << "\nClosing connection with fd " << client->connected_fd << std::endl;

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
			//TODO handle error
			perror("\nERROR\nDefaultServer.sendResponse(): kevent()/epoll_ctl()");
			exit(EXIT_FAILURE);
		}

		// erase client from the map of clients and delete it from memory
		clients.erase(connected_fd);
		delete client;
	}
}
