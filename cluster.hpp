#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>		//pair
#include <sys/epoll.h>	//epoll

#include "server.hpp"

#define MAX_EVENTS 1000

class Cluster
{
public:
	// aliases
	typedef std::pair<std::string,unsigned short>	host_port_type;

private:
	// attributes
	std::vector<Server>					servers_v;
	std::map< host_port_type, Server& >	defaultServers_m;
	int									epoll_fd;
	struct epoll_event					events;
	struct epoll_event					ret_events[MAX_EVENTS];
	int									num_ready_fds;

public:
	// constructor
	Cluster(std::string config_file)	//NOTE: if the config file is not valid, then default config file is used
	{
		//TODO parsing config file e initialization of cluster
	}

	// destructor
	~Cluster() {}

	// getters
	int const getEpollFd() const { return epoll_fd; }

	// run
	void run()
	{
		epoll_fd = epoll_create1(0);
		if (epoll_fd < 0)
		{
			//TODO handle error
			// perror("cluster.run(): epoll_create1")
		}
		for (std::vector<Server>::iterator it = servers_v.begin(); it != servers_v.end(); ++it)
		{
			it->startListening();
			events.events = EPOLLIN;
			events.data.fd = it->getListeningFd();
			events.data.ptr = (void *)&(*it);
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, it->getListeningFd(), &events) == -1)
			{
				//TODO handle error
				// perror("cluster.run(): epoll_ctl")
			}
		}
		// connect and communicate with clients
		while (1)
		{
			num_ready_fds = epoll_wait(epoll_fd, ret_events, MAX_EVENTS, -1);
			if (num_ready_fds == -1)
			{
				//TODO handle error
				// perror("cluster.run(): epoll_wait")
			}
			for (int i = 0; i < num_ready_fds; ++i)
			{
				Base *base_ptr = static_cast<Base *>(ret_events[i].data.ptr);
				if (dynamic_cast<Server *>(base_ptr))
				{
					Server *server = static_cast<Server *>(base_ptr);
					server->connectToClient();
				}
				else if (dynamic_cast<ConnectedClient *>(base_ptr))
				{
					//TODO read from connected client
				}
			}
		}
	}

};
