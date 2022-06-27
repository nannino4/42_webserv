#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#ifdef __MACH__ // __MACH__ library
#include <sys/event.h>	//kqueue kevent
#endif
#ifdef __linux__ // linux library
#include <sys/epoll.h>  //epoll for linux
#endif
#include "default_server.hpp"

#define DEF_CONF "/config_files/default.conf"
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
	#ifdef __MACH__
	int								kqueue_fd;
	struct kevent					triggered_events[N_EVENTS];
	#endif
	#ifdef __linux__
	int								epoll_fd;
	struct epoll_event				triggered_events[N_EVENTS];
	#endif

public:
	// constructor
	// Cluster(std::string config_file_name);

	// constructor per DEBUG
	Cluster();

	// destructor
	~Cluster();

	// getters
	#ifdef __MACH__
	int	getKqueueFd() const;
	#endif
	#ifdef __linux__
	int	getEpollFd() const;
	#endif
	// run
	void run();

};
