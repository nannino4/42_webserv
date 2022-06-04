#include "server.hpp"

// getters
struct sockaddr_in const	&Server::getAddress() const { return server_addr; }
unsigned short const		Server::getBacklog() const { return backlog; }
int const					Server::getListeningFd() const { return socket->getFd(); }
int const					Server::getKqueueFd() const { return cluster.getKqueueFd(); }

// constructor
Server::Server(Cluster const &cluster, bool default_server, unsigned short backlog, std::string content) :
cluster(cluster), default_server(default_server), backlog(backlog)
{
	//TODO parse content and initialize server
}

// destructor
Server::~Server()
{
	for (std::vector<ConnectedClient *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->~ConnectedClient();
		delete *it;
	}
}

// start listening
void Server::startListening()
{
	socket = new Socket(*this);
}

// connect to client
void Server::connectToClient()
{
	ConnectedClient *newClient = new ConnectedClient(*this);
	clients.push_back(newClient);
}

// communicate with client
void Server::getRequest(ConnectedClient &client)
{
	int read_bytes = recv(client.getConnectedFd(), client.getBuf(), BUFFER_SIZE, 0);
	if (read_bytes == -1)
	{
		//TODO handle error
		// perror("ERROR\nServer: readFromClient: recv");
	}
	client.getMessage() += client.getBuf();
	bzero(client.getBuf(), BUFFER_SIZE);
	if (read_bytes < BUFFER_SIZE)
		giveResponse(client);
}

void Server::giveResponse(ConnectedClient &client)
{
	std::cout << client.getMessage() << std::endl;	//DEBUG
	//TODO parse request - client.getMessage() - and create response
}
