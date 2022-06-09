#pragma once

#include <string>
#include <vector>
#include <map>

#include "cluster.hpp"
// #include "location.hpp"
#include "socket.hpp"

#define BUFFER_SIZE BUFSIZ

class Cluster;

class Base
{
protected:
	// attributes
	Socket	*socket;

public:
	// constructor
	Base();

	// destructor
	virtual ~Base();
};

class Server : public Base
{
private:
	// attributes
	Cluster	const						&cluster;

	std::vector<std::string>			names;
	struct sockaddr_in					server_addr;
	std::map<int, std::string>			error_pages;
	// size_t								client_body_size;
	// std::map<std::string,Location>		locations;

	std::vector<ConnectedClient *>		clients;
	unsigned short						backlog;

public:
	// constructor
	// Server(Cluster const &cluster, unsigned short backlog, std::ifstream config_file);
	// DEBUG constructor
	Server(Cluster const &cluster, unsigned short backlog);

	// destructor
	~Server();

	// getters
	struct sockaddr_in const	&getAddress() const;
	unsigned short				getBacklog() const;
	int							getListeningFd() const;
	int							getKqueueFd() const;

	// start listening
	void startListening();

	// connect to client
	void connectToClient();

	// communicate with client
	void getRequest(ConnectedClient &client);
	void giveResponse(ConnectedClient &client);

};

class ConnectedClient : public Base
{
private:
	// attributes
	Server				&server;
	struct sockaddr_in	client_addr;
	char				buf[BUFSIZ];
	std::string			message;

public:
	// constructor
	ConnectedClient(Server &server);

	// destructor
	~ConnectedClient();

	// getters
	int 				getListeningFd() const;
	int 				getConnectedFd() const;
	struct sockaddr_in	&getAddress();
	int					getKqueueFd() const;
	char				*getBuf();
	std::string			&getMessage();

	// communicate with server
	void communicate();

};
