#include "default_server.hpp"

// constructor
DefaultServer::DefaultServer(int const &kqueue_fd, unsigned int backlog) : Server(kqueue_fd), backlog(backlog)
{
	bzero(buf, BUFFER_SIZE);
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	error_pages.insert(std::pair<int,std::string>(404, "./error_pages/404.html"));	//TODO aggiungi altre pagine di errore
	std::cout << "+ un nuovo default server e' stato creato" << std::endl;	//DEBUG
}

// destructor
DefaultServer::~DefaultServer()
{
	if (listening_fd != -1)
		close(listening_fd);
	std::cout << "- un default server e' stato distrutto" << std::endl;		//DEBUG
}

// getters
struct sockaddr_in const	&DefaultServer::getAddress() const { return server_addr; }
unsigned int const			&DefaultServer::getBacklog() const { return backlog; }
int const					&DefaultServer::getListeningFd() const { return listening_fd; }
int const					&DefaultServer::getKqueueFd() const { return kqueue_fd; }

// communication
void DefaultServer::startListening()
{
	listening_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_fd < 0)
	{
		//TODO handle errors
		perror("ERROR\nDefaultServer.startListening(): socket()");
		exit(EXIT_FAILURE);
	}
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

	//DEBUG
	std::cout << "DefaultServer: kqueue_fd = " << kqueue_fd << std::endl;

	struct kevent event;
	EV_SET(&event, listening_fd, EVFILT_READ, EV_ADD, 0, 0, (void *)this);		// ident = listening_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)				// filter = READ
	{																			// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nDefaultServer.startListening(): kevent()");
		exit(EXIT_FAILURE);
	}

	//DEBUG
	std::cout << "listening on fd = " << listening_fd << "\nport = " << ntohs(server_addr.sin_port) << "\nip = " << inet_ntoa(server_addr.sin_addr) << std::endl << std::endl;
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
	std::cout << "connected to fd = " << connected_fd << "\nport = " << ntohs(client_addr.sin_port) << "\nip = " << inet_ntoa(client_addr.sin_addr) << std::endl << std::endl;

	// create new ConnectedClient
	clients.insert(std::pair<int,ConnectedClient>(connected_fd, ConnectedClient(connected_fd, client_addr)));

	//DEBUG
	std::cout << "there are now " << clients.size() << " clients on this server with listening fd = " << listening_fd << std::endl;

	//DEBUG
	std::cout << "DefaultServer: kqueue_fd = " << kqueue_fd << std::endl;

	// add new connected_fd to kqueue for READ monitoring
	struct kevent event;
	EV_SET(&event, connected_fd, EVFILT_READ, EV_ADD, 0, 0, (void *)this);		// ident = connected_fd
	if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)				// filter = READ
	{																			// udata = DefaultServer*
		//TODO handle error
		perror("ERROR\nDefaultServer.connectToClient(): kevent()");
		exit(EXIT_FAILURE);
	}
}

void DefaultServer::receiveRequest(struct kevent const event)
{
	int connected_fd = event.ident;

	// find client corresponding to connected_fd
	if (clients.find(connected_fd) == clients.end())
	{
		//TODO handle error
		// strerror("ERROR\nDefaultServer.receiveRequest(): could not find connected_fd among clients")
	}
	ConnectedClient &client = clients.find(connected_fd)->second;

	// read from connected_fd into client->message
	int read_bytes = recv(connected_fd, buf, BUFFER_SIZE, 0);
	if (read_bytes == -1)
	{
		//TODO handle error
		perror("ERROR\nDefaultServer.receiveRequest(): recv");
		exit(EXIT_FAILURE);
	}
	client.message += buf;
	bzero(buf, BUFFER_SIZE);
	if (read_bytes < BUFFER_SIZE && event.filter & EV_EOF)
		dispatchRequest(client);
}

void DefaultServer::dispatchRequest(ConnectedClient &client)
{
	//TODO dispatch the request to the corresponding server, based on the 'host' value
	prepareResponse(client);	//DEBUG
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

	//print response for DEBUG reason
	std::cout << "THE RESPONSE TO FD " << connected_fd << " IS:\n" << client.message << std::endl;	//DEBUG
	
	int size = ((unsigned long)(client.message_pos + buf_size) > client.message.size()) ? (client.message.size() - client.message_pos) : buf_size;
	send(connected_fd, client.message.substr(client.message_pos, client.message_pos + buf_size).c_str(), size, 0);			//TODO check that sizes are correct!
	if (size != buf_size)
	{
		// remove connected_fd from kqueue
		struct kevent event;
		EV_SET(&event, connected_fd, 0, EV_DELETE, 0, 0, 0);
		if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
		{
			//TODO handle error
			perror("ERROR\nDefaultServer.sendResponse(): kevent()");
			exit(EXIT_FAILURE);
		}

		clients.erase(connected_fd);
	}
}
