#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <sys/event.h>	//kqueue kevent

#include "server.hpp"

#define DEF_CONF "/config_files/default.conf"
#define N_EVENTS 1000
#define BACKLOG_SIZE 128

class Server;

class Cluster
{
private:
	// aliases
	typedef std::pair<std::string,unsigned short>	address;
private:
	// attributes
	std::map<address,Server>			default_servers;
	int									kqueue_fd;
	struct kevent						event;
	struct kevent						triggered_events[N_EVENTS];

public:
	// constructor
	// Cluster(std::string config_file_name);

	// constructor per DEBUG
	Cluster();

	// destructor
	~Cluster();

	// getters
	int	getKqueueFd() const;

	// run
	void run();

};
