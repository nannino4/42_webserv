#include "default_server.hpp"

// default constructor
DefaultServer::DefaultServer(int const &kqueue_fd, unsigned int backlog, std::string &config_file, int &pos) : Server(kqueue_fd), backlog(backlog)
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
DefaultServer::DefaultServer(DefaultServer const &other) : Server(other) { *this = other; }

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
}

// operator overload
std::ostream &operator<<(std::ostream &os, DefaultServer const &def_serv)
{
	os << "\nDefaultServer introducing itself:\n";
	os << "backlog:\n" << def_serv.backlog << std::endl;
	os << "server_addr:\n" << inet_ntoa(def_serv.server_addr.sin_addr) << ":" << ntohs(def_serv.server_addr.sin_port) << std::endl;
	os << "virtual_servers:\n";
	for (std::vector<Server>::const_iterator it = def_serv.virtual_servers.begin(); it != def_serv.virtual_servers.end(); ++it)
	{
		os << *it << std::endl;
	}
	os << "clients:\n";
	for (std::map<int,ConnectedClient>::const_iterator it = def_serv.clients.begin(); it != def_serv.clients.end(); ++it)
	{
		os << it->second << std::endl;
	}
	os << "listening_fd:\n" << def_serv.listening_fd << std::endl;
	os << "\nDefaultServer introduction is over" << std::endl;
	return os;
}

// getters
DefaultServer::address	DefaultServer::getAddress() const { return address(server_addr.sin_addr.s_addr, server_addr.sin_port); }
unsigned int const		&DefaultServer::getBacklog() const { return backlog; }
int const				&DefaultServer::getListeningFd() const { return listening_fd; }
int const				&DefaultServer::getKqueueFd() const { return kqueue_fd; }

// initialization
// add virtual server
void DefaultServer::addVirtualServer(DefaultServer tmp_serv)
{
	Server 						newServer = (Server)tmp_serv;
	std::vector<std::string>	tmp_names = newServer.getNames();

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
	virtual_servers.push_back(newServer);
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

	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, listening_fd, EVFILT_READ, EV_ADD, 0, 0, (void *)this);		// ident = listening_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)				// filter = READ
	{																			// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nDefaultServer.startListening(): kevent()");
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

	// //debug
	// fcntl(connected_fd, F_SETFL, O_NONBLOCK);

	//DEBUG
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer:connectToClient()\n\nconnected to fd = " << connected_fd << \
			"\nport = " << ntohs(client_addr.sin_port) << \
			"\nip = " << inet_ntoa(client_addr.sin_addr) << std::endl << std::endl;

	// create new ConnectedClient
	clients.insert(std::pair<int,ConnectedClient>(connected_fd, ConnectedClient(connected_fd, client_addr)));

	// add new connected_fd to kqueue for READ monitoring
	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, connected_fd, EVFILT_READ, EV_ADD, 0, 0, this);		// ident = connected_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)		// filter = READ
	{																	// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nDefaultServer.connectToClient(): kevent()");
		exit(EXIT_FAILURE);
	}

	//debug
	std::cout << "\nThe event with ident = " << connected_fd << " and filter EVFILT_READ has been added to kqueue" << \
			"\nthere are currently " << clients.size() << " clients connected\n" << std::endl;
}

void DefaultServer::receiveRequest(struct kevent const event)
{
	//debug
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer.receiveRequest():" << std::endl;

	int connected_fd = event.ident;

	// find client corresponding to connected_fd
	if (clients.find(connected_fd) == clients.end())
	{
		//TODO handle error
		std::cout << "ERROR\nDefaultServer.receiveRequest(): could not find connected_fd among clients" << std::endl;
		exit(EXIT_FAILURE);
	}
	ConnectedClient &client = clients.find(connected_fd)->second;

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
	client.message += buf;

	//debug
	std::cout << "\nreceived data = \"" << buf << "\"" \
			"\nreceived request = \"" << client.message << "\"" \
			"\nread_bytes = " << read_bytes << \
			"\nBUFFER_SIZE - 1 = " << BUFFER_SIZE - 1 << \
			"\nEOF = " << (event.flags & EV_EOF) << std::endl;

	bzero(buf, BUFFER_SIZE);

	if (read_bytes < (BUFFER_SIZE - 1))// && event.flags & EV_EOF)
	{
		//DEBUG
		std::cout << "\nThe whole request has been received" << std::endl;

		// remove connected_fd to kqueue from READ monitoring
		struct kevent event;
		bzero(&event, sizeof(event));
		EV_SET(&event, client.connected_fd, EVFILT_READ, EV_DELETE, 0, 0, this);
		if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
		{
			//TODO handle error
			perror("ERROR\nDefaultServer.receiveRequest: kevent()");
			exit(EXIT_FAILURE);
		}

		//debug
		std::cout << "\nThe event with ident = " << client.connected_fd << " and filter EVFILT_READ has been removed from kqueue\n" << std::endl;
		dispatchRequest(client);
	}
}

void DefaultServer::dispatchRequest(ConnectedClient &client)
{
	//TODO dispatch the request to the corresponding server, based on the 'host' value
	Request request(client.message);
	
	Server *serverRequested = this;
	for (std::vector<Server>::iterator it = virtual_servers.begin(); it != virtual_servers.end(); ++it)
	{
		if (it->isName(request.getHostname()))
			serverRequested = &(*it);
	}
	
	//debug
	// prepareResponse(client, serverRequested);
	serverRequested->prepareResponse(client, request);
	
	// add connected_fd to kqueue for WRITE monitoring
	struct kevent event;
	bzero(&event, sizeof(event));
	EV_SET(&event, client.connected_fd, EVFILT_WRITE, EV_ADD, 0, 0, this);					// ident = connected_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)							// filter = WRITE
	{																						// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nDefaultServer.dispatchRequest: kevent()");
		exit(EXIT_FAILURE);
	}

	//debug
	std::cout << "\nThe event with ident = " << client.connected_fd << " and filter EVFILT_WRITE has been added to kqueue\n" << std::endl;
}

void DefaultServer::sendResponse(int const connected_fd, int const buf_size)
{
	// find client corresponding to connected_fd
	if (clients.find(connected_fd) == clients.end())
	{
		//TODO handle error
		std::cerr << "ERROR\nDefaultServer.sendResponse(): could not find connected_fd among clients" << std::endl;
		exit(EXIT_FAILURE);
	}
	ConnectedClient &client = clients.find(connected_fd)->second;

	//DEBUG
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "\nDefaultServer:sendResponse():\n\nTHE RESPONSE TO FD " << connected_fd << " IS: \"" << client.message << "\"" << std::endl;	//DEBUG
	
	int size = ((unsigned long)(client.message_pos + buf_size) > client.message.size()) ? (client.message.size() - client.message_pos) : buf_size;
	if (send(connected_fd, client.message.substr(client.message_pos, client.message_pos + size).c_str(), size, 0) == -1)
	{
		//TODO handle error
		std::cerr << "ERROR\nDefaultServer.sendResponse(): send()" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "\nsent some data to fd = " << connected_fd << \
			"\nmessage to be sent = \"" << client.message << "\"" << \
			"\nmessage position = " << client.message_pos << \
			"\nmessage size = " << client.message.size() << \
			"\nsize of data remaining to be sent = " << client.message.size() - client.message_pos << \
			"\nbuffer size = " << buf_size << \
			"\nsize of data sent = " << size << \
			"\ndata sent = \"" << client.message.substr(client.message_pos, client.message_pos + size) << "\"\n" << std::endl;
	client.message_pos += size;
	if (size < buf_size)
	{
		//debug
		std::cout << "\nThe whole response has been sent" << std::endl;
		// remove connected_fd from kqueue
		struct kevent event;
		bzero(&event, sizeof(event));
		EV_SET(&event, connected_fd, EVFILT_WRITE, EV_DELETE, 0, 0, this);
		if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
		{
			//TODO handle error
			// perror("ERROR\nDefaultServer.sendResponse(): kevent()");
			std::cout << "ERROR\nDefaultServer.sendResponse(): kevent(): errno = " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// close connected_fd and erase it from the map of clients
		if (close(connected_fd) == -1)
		{
			//TODO handle error
			std::cerr << "ERROR\nDefaultServer.sendResponse(): close()" << std::endl;
			exit(EXIT_FAILURE);
		}
		clients.erase(connected_fd);

		//debug
		std::cout << "\nThe event with ident = " << connected_fd << " and filter EVFILT_WRITE has been removed from kqueue" << \
				"\nthere are currently " << clients.size() << " clients connected\n" << std::endl;
	}
}
