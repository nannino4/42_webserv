#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>		//pair
#include <sys/event.h>	//kqueue kevent
#include <fcntl.h>		//open()

#include "server.hpp"

#define DEF_CONF "/conf/default.conf"
#define N_EVENTS 1000
#define BACKLOG_SIZE 128

class Cluster
{
private:
	// attributes
	std::vector<Server>					servers;
	int									kqueue_fd;
	struct kevent						event;
	struct kevent						triggered_events[N_EVENTS];

public:
	// constructor
	Cluster(std::string config_file_name);

	// destructor
	~Cluster();

	// getters
	int const			getKqueueFd() const;

	// run
	void run();

};
