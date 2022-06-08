#include "server.hpp"

// constructor
Server::Server(Cluster const &cluster, bool default_server, unsigned short backlog, std::ifstream config_file) :
cluster(cluster), backlog(backlog)
{
	std::stringstream	line;
	std::string			tmp;
	std::string			directive;
	std::string			path;

	std::getline(config_file, tmp);
	while (tmp.compare("}"))	//TODO fai meglio
	{
		if (!tmp.empty())
		{
			line.str(tmp);
			line >> directive;
			if (!directive.compare("location"))
			{
				line >> path;
				line >> tmp;
				if (tmp.compare("{"))
				{
					//TODO handle error
					// std::cerr << "ERROR\nServer: parsing config_file: The location directive should only be followed by the path and '{'" << std::endl;
					// throw()
				}
				else
				{
					line >> tmp;
					if (tmp.empty())
					{
						locations[path] = Location(config_file);
					}
					else
					{
						//TODO handle error
						// std::cerr << "ERROR\nServer: parsing config_file: The location directive should only be followed by the path and '{'" << std::endl;
						// throw()
					}
				}
			}
			else
			{
				//TODO check for other directives
			}
		}
		std::getline(config_file, tmp);
	}
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

// getters
struct sockaddr_in const	&Server::getAddress() const { return server_addr; }
unsigned short const		Server::getBacklog() const { return backlog; }
int const					Server::getListeningFd() const { return socket->getFd(); }
int const					Server::getKqueueFd() const { return cluster.getKqueueFd(); }

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
