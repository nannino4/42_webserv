#pragma once

#include <string>
#include <map>
#include <iostream>

#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <cstring>		//bzero
#include <unistd.h>		//close
#include <fcntl.h>		//fcntl

#ifdef __MACH__
	#include <sys/event.h>	//kqueue kevent
#elif defined(__linux__)
	#include <sys/epoll.h>  //epoll for linux
#endif

#include "server.hpp"
#include "event.hpp"
#include "utils.hpp"

#define DEF_ADDR INADDR_ANY
#define DEF_PORT 8000
#define BUFFER_SIZE 999
#define TIMEOUT 5000
#define REQUEST_SIZE_LIMIT (1000 * BUFFER_SIZE)

class DefaultServer : public Server
{
private:
	// aliases
	typedef std::pair<in_addr_t,in_port_t>	address;

private:
	// int const							&kqueue_epoll_fd;			// inherited from Server
	// std::vector<std::string>				names;
	// std::map<int, std::string>			error_pages;
	// size_t								client_body_size;
	// std::map<std::string,Location>		locations;


	// attributes
	unsigned int						backlog;
	// configuration parameters
	struct sockaddr_in					server_addr;
	std::vector<Server>					virtual_servers;
	// communication
	std::map<int,ConnectedClient&>		clients;
	int									listening_fd;
	Event								triggered_event;
	char								buf[BUFFER_SIZE];

public:
	// default constructor
	DefaultServer(int const &kqueue_epoll_fd, unsigned int backlog, std::string &config_file, int &pos);
	// copy constructor
	DefaultServer(DefaultServer const &other);
	// assign operator
	DefaultServer &operator=(DefaultServer const &other);

	// destructor
	~DefaultServer();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, DefaultServer const &def_serv);

	// getters
	address				getAddress() const;
	std::string			getIp() const;
	std::string			getPort() const;
	unsigned int const	&getBacklog() const;
	int const			&getListeningFd() const;

	// initialization
	void addVirtualServer(DefaultServer &newServer);

	// communication
	void startListening();
	void connectToClient();
	void disconnectFromClient(ConnectedClient *client);
	void receiveRequest(Event *current_event);
	void dispatchRequest(ConnectedClient *client);
	// void prepareResponse(ConnectedClient &client);		// inherited from Server
	void sendResponse(Event *current_event);
	void closeTimedOutConnections();

private:
	// initialization
	void parseDirectives(std::stringstream &stream);
	void parseListen(std::stringstream &stream);
	void parseName(std::stringstream &stream);
	void parseBodySize(std::stringstream &stream);
	void parseErrorPage(std::stringstream &stream);

};
