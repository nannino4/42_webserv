#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

#ifdef __MACH__
	#include <sys/event.h>	//kqueue kevent
#elif defined(__linux__)
	#include <sys/epoll.h>  //epoll for linux
#endif

#include <arpa/inet.h>	//inet_addr

#include "default_server.hpp"
#include "utils.hpp"

#define DEF_CONF "./config_files/default.conf"
#define N_EVENTS 1000
#define BACKLOG_SIZE 128
#define HOW_OFTEN 100

class Cluster
{
private:
	// aliases
	typedef std::pair<in_addr_t,in_port_t>	address;

private:
	// attributes
	std::map<address,DefaultServer&>	default_servers;
	int									kqueue_epoll_fd;

	#ifdef __MACH__
		struct kevent					triggered_events[N_EVENTS];
	#elif defined(__linux__)
		struct epoll_event				triggered_events[N_EVENTS];
	#endif

public:
	// constructor
	Cluster(std::string config_file_name);

	// destructor
	~Cluster();

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, Cluster const &cluster);

	// getters
	int	getKqueueEpollFd() const;

	// run
	void run();

private:
	//--------- utils
	// init
	std::ifstream	&openConfigFile(std::string config_file_name);
	std::string		fileToString(std::ifstream &config_file);
	
}; // end of cluster class

