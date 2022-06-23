#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <sys/event.h>	//kqueue kevent
#include <arpa/inet.h>	//inet_addr

#include "default_server.hpp"

#define DEF_CONF "./config_files/default.conf"
#define N_EVENTS 1000
#define BACKLOG_SIZE 128

class Cluster
{
private:
	// aliases
	typedef std::pair<in_addr_t,in_port_t>	address;

private:
	// attributes
	std::map<address,DefaultServer>	default_servers;
	int								kqueue_fd;
	struct kevent					triggered_events[N_EVENTS];

public:
	// constructor
	Cluster(std::string config_file_name);

	// // constructor per DEBUG
	// Cluster();

	// destructor
	~Cluster();

	// getters
	int	getKqueueFd() const;

	// run
	void run();

private:
	// utils
	std::ifstream	&openConfigFile(std::string config_file_name);
	std::string		fileToString(std::ifstream &config_file);
	
	// initialization
	// location
	void parseLocationBlock();
	void parseLocationDirectives();

};
