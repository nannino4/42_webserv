#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>		//pair
#include <sys/event.h>	//kqueue kevent
#include <fcntl.h>		//open()

#include "server.hpp"

#define DEF_CONF "/conf/default.conf"
#define N_EVENTS 1000

class Cluster
{
public:
	// aliases
	typedef std::pair<std::string,unsigned short>	host_port_type;

private:
	// attributes
	std::vector<Server>					servers_v;
	std::map< host_port_type, Server& >	defaultServers_m;
	int									kqueue_fd;
	struct kevent						event;
	struct kevent						triggered_events[N_EVENTS];
	int									num_ready_fds;

public:
	// constructor
	Cluster(std::string config_file)	//NOTE: if the config file is not valid, then default config file is used
	{
		int conf_fd;

		conf_fd = open(config_file.c_str(), O_RDONLY);
		if (conf_fd == -1)
		{
			std::cout << "WARNING\n" << config_file << " is not a valid configuration file. The default configuration file \"default.conf\" is used instead" << std::endl;
			conf_fd = open(DEF_CONF, O_RDONLY);
			if (conf_fd == -1)
			{
				//TODO handle error
				// perror("ERROR\nCluster: trying to open default.conf with open()")
			}
		}
		//TODO parsing config file e initialization of cluster
	}

	// destructor
	~Cluster() {}

	// getters
	int const getKqueueFd() const { return kqueue_fd; }

	// run
	void run()
	{
		kqueue_fd = kqueue();
		if (kqueue_fd == -1)
		{
			//TODO handle error
			// perror("ERROR\ncluster.run(): epoll_create1")
		}
		// make servers listen and add them to kqueue
		for (std::vector<Server>::iterator it = servers_v.begin(); it != servers_v.end(); ++it)
		{
			it->startListening();
			EV_SET(&event, it->getListeningFd(), EVFILT_READ, EV_ADD, 0, 0, (void *)&(*it));
			if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
			{
				//TODO handle error
				// perror("ERROR\ncluster.run(): adding listeningFd to kqueue with kevent()")
			}
		}
		// let servers connect and communicate with clients
		while (1)
		{
			num_ready_fds = kevent(kqueue_fd, nullptr, 0, triggered_events, N_EVENTS, nullptr);
			if (num_ready_fds == -1)
			{
				//TODO handle error
				// perror("ERROR\ncluster.run(): getting triggered events with kevent()")
			}
			for (int i = 0; i < num_ready_fds; ++i)
			{
				Base *base_ptr = static_cast<Base *>(triggered_events[i].udata);
				if (dynamic_cast<Server *>(base_ptr))
				{
					Server *server = (Server *)base_ptr;
					server->connectToClient();
				}
				else if (dynamic_cast<ConnectedClient *>(base_ptr))
				{
					ConnectedClient *client = (ConnectedClient *)base_ptr;
					client->communicate();
				}
			}
		}
	}

};
