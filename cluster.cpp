#include "cluster.hpp"

Cluster::Cluster(std::string config_file_name)	//NOTE: if the config file is not valid, then default config file is used
{
	std::ifstream config_file(config_file_name);

	// check that config_file_name exists and is valid, otherwise use DEF_CONF instead
	if (!config_file.is_open() && !config_file_name.compare(DEF_CONF))
	{
		//TODO handle error
		// perror("ERROR/nCluster: trying to open default.conf")
		// throw();
	}
	if (!config_file.is_open() && config_file_name.compare(DEF_CONF))
	{
		std::cout << "WARNING\n" << config_file << " is not a valid configuration file. The default configuration file " << DEF_CONF << " is used instead" << std::endl;
		config_file.open(DEF_CONF);
		if (!config_file.is_open())
		{
			//TODO handle error
			// perror("ERROR\nCluster: trying to open default.conf")
			// throw();
		}
	}
	// parse config_file in search for 'server' directives to create servers
	std::stringstream	line;
	std::string			tmp;
	std::string			directive;

	std::getline(config_file, tmp);
	while (config_file.good())
	{
		if (!tmp.empty())
		{
			line.str(tmp);
			line >> directive;
			if (!directive.compare("server"))
			{
				line >> tmp;
				if (tmp.compare("{"))
				{
					//TODO handle error
					// std::cerr << "ERROR\nCluster: parsing config_file: The server directive should only be followed by space and \"{\"" << std::endl;
					// throw()
				}
				else
				{
					line >> tmp;
					if (tmp.empty())
					{
						servers.push_back(Server(*this, BACKLOG_SIZE, config_file));
					}
					else
					{
						//TODO handle error
						// std::cerr << "ERROR\nCluster: parsing config_file: The server directive should only be followed by space and \"{\"" << std::endl;
						// throw()
					}
				}
			}
			else
			{
				//TODO handle error
				// std::cerr << "ERROR\nCluster: parsing config_file: directive " << directive << " is invalid" << std::endl;
				// throw()
			}
		}
		std::getline(config_file, tmp);
	}
}

// constructor per DEBUG
Cluster::Cluster()
{
	// std::cout << "inizio constructor cluster" << std::endl;
	default_servers.insert(std::pair<address,DefaultServer>(address(inet_addr("0.0.0.0"), htons(8080)), DefaultServer(kqueue_fd, BACKLOG_SIZE)));
	// std::cout << "fine constructor cluster" << std::endl;
	// std::cout << "ci sono " << default_servers.size() << " default servers nel cluster" << std::endl;
}

// destructor
Cluster::~Cluster() {}

// getters
int Cluster::getKqueueFd() const
{
	return kqueue_fd;
}

// run
void Cluster::run()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
	{
		//TODO handle error
		perror("ERROR\ncluster.run(): epoll_create1()");
		exit(EXIT_FAILURE);
	}

	// make servers listen and add them to kqueue
	for (std::map<address,DefaultServer>::iterator it = default_servers.begin(); it != default_servers.end(); ++it)
	{
		it->second.startListening();
	}

	// let servers connect and communicate with clients
	int	num_ready_fds;
	while (1)
	{
		num_ready_fds = kevent(kqueue_fd, nullptr, 0, triggered_events, N_EVENTS, nullptr);

		//debug
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "\nCluster.run():\n\nkevent() returned " << num_ready_fds << " events\n" << std::endl;

		if (num_ready_fds == -1)
		{
			//TODO handle error
			perror("ERROR\ncluster.run(): kevent()");
			exit(EXIT_FAILURE);
		}

		// handle the event triggered on the monitored fds
		for (int i = 0; i < num_ready_fds; ++i)
		{
			//debug
			std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
			std::cout << "\nnew 'for loop' starting with index = " << i << std::endl;

			DefaultServer *default_server = (DefaultServer *)triggered_events[i].udata;

			if (triggered_events[i].flags == EV_ERROR)
			{
				//debug
				std::cout << "the event has flags = EV_ERROR\n" << std::endl;

				//the connected_fd got an error_filter
				//TODO handle error
				std::cout << "ERROR\nan event reported EV_ERROR in flags" << std::endl;
				strerror(triggered_events[i].data);
			}
			else if (triggered_events[i].filter == EVFILT_WRITE)
			{
				//debug
				std::cout << "the event has filter = EVFILT_WRITE\n" << std::endl;

				// response can be sent to connected_fd
				default_server->sendResponse(triggered_events[i].ident, triggered_events[i].data);
			}
			else if (triggered_events[i].filter == EVFILT_READ)
			{
				//debug
				std::cout << "the event has filter = EVFILT_READ\n" << std::endl;

				if (triggered_events[i].ident == (unsigned long)default_server->getListeningFd())
				{
					//debug
					std::cout << "the event has fd = listening_fd\n" << std::endl;

					// listening_fd ready to accept a new connection from client
					default_server->connectToClient();
				}
				else
				{
					//debug
					std::cout << "the event has fd = " << triggered_events[i].ident << " != listening_fd\n" << std::endl;

					// request can be received from connected_fd
					default_server->receiveRequest(triggered_events[i]);
				}
			}

			//debug
			std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
			std::cout << "\nCluster.run():\n\n'for loop' with index = " << i << " is over. Maximum index = " << num_ready_fds - 1 << std::endl << std::endl;
		}
	}
}
