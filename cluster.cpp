#include "cluster.hpp"

// default constructor
Cluster::Cluster(std::string config_file_name)	//NOTE: if the config file is not valid, then default config file is used
{

#ifdef __MACH__
	kqueue_epoll_fd = kqueue();
#elif defined(__linux__)
	kqueue_epoll_fd = epoll_create1(0);
#endif
	if (kqueue_epoll_fd == -1)
	{
		//TODO handle error
		perror("\nERROR\ncluster.run(): kqueue()/epoll_create1()");
		exit(EXIT_FAILURE);
	}

	std::ifstream config_file(config_file_name);
	std::stringstream	stream;
	std::string			whole_file("");
	std::string			directive;
	int					pos = 0;
	int					found_pos;

	// check that config_file_name exists and is valid, otherwise use DEF_CONF instead
	if (!config_file.is_open() && !config_file_name.compare(DEF_CONF))
	{
		//TODO handle error
		perror("\nERROR\nCluster: trying to open the default configuration file");
		exit(EXIT_FAILURE);
		// throw();
	}
	if (!config_file.is_open() && config_file_name.compare(DEF_CONF))
	{
		std::cout << "\nWARNING\n\"" << config_file_name << "\" is not a valid configuration file. The default configuration file " << DEF_CONF << " is used instead" << std::endl;
		config_file.open(DEF_CONF);
		if (!config_file.is_open())
		{
			//TODO handle error
			perror("\nERROR\nCluster: trying to open the default configuration file");
			exit(EXIT_FAILURE);
			// throw();
		}
	}

	whole_file = fileToString(config_file);
	// now whole_file contains the whole file

	// parse whole_file searching for 'server' blocks
	while ((unsigned long)(found_pos = whole_file.find_first_of('{', pos)) != std::string::npos)
	{
		stream.clear();
		stream.str(whole_file.substr(pos, (found_pos - pos)));
		pos = (found_pos + 1);
		directive.clear();
		stream >> directive;

		// check that stream didn't fail reading
		if (stream.fail())
		{
			//TODO handle error
			std::cerr << "\nERROR\nCluster::Cluster(): stream reading failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (directive.compare("server"))
		{
			//TODO handle error
			std::cerr << "\nERROR\nCluster::Cluster(): parsing config_file: \"" << directive << "\" is an invalid directive at the current scope" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			directive.clear();
			stream >> directive;
			if (!directive.empty())
			{
				//TODO handle error
				std::cerr << "\nERROR\nCluster::Cluster(): parsing config_file: found invalid text between the directive \"server\" and '{'" << std::endl;
				exit(EXIT_FAILURE);
			}
			else
			{
				DefaultServer *new_server = new DefaultServer(kqueue_epoll_fd, BACKLOG_SIZE, whole_file, pos);		// still can't know if it's going to be a Server or DefaultServer
	
				//check whether there is already a default server with the same address:port combination
				if (default_servers.find(new_server->getAddress()) == default_servers.end())
				{
					// adding new default server
					default_servers.insert(std::pair<address,DefaultServer&>(new_server->getAddress(), *new_server));
				}
				else
				{
					// adding new virtual server because there is already a default server with same address:port combination
					(default_servers.find(new_server->getAddress()))->second.addVirtualServer(*new_server);
					delete new_server;
				}
			}
		}
	}
	stream.str(whole_file.substr(pos));
	stream >> std::ws;
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nCluster::Cluster(): found invalid text after the last '}'" << std::endl;
	}
	std::cout << "Cluster initialization has been completed.\n" << *this << std::endl;		//debug
}

// destructor
Cluster::~Cluster()
{
	for (std::map<Cluster::address,DefaultServer&>::iterator it = default_servers.begin(); it != default_servers.end(); ++it)
	{
		it->second.~DefaultServer();
		delete &it->second;
	}
}

// operator overload
std::ostream &operator<<(std::ostream &os, Cluster const &cluster)
{
	os << "\nCluster introducing itself:\n";
	os << "kqueue_epoll_fd: " << cluster.kqueue_epoll_fd << std::endl;
	os << "default servers: " << cluster.default_servers.size() << std::endl;
	for (std::map<Cluster::address,DefaultServer&>::const_iterator it = cluster.default_servers.begin(); it != cluster.default_servers.end(); ++it)
	{
		os << it->second;
	}
	os << "Cluster introduction is over\n" << std::endl;
	return os;
}

// getters
int Cluster::getKqueueEpollFd() const
{
	return kqueue_epoll_fd;
}

// run
void Cluster::run()
{
	// make servers listen and add them to kqueue
	for (std::map<Cluster::address,DefaultServer&>::iterator it = default_servers.begin(); it != default_servers.end(); ++it)
	{
		it->second.startListening();
	}

	// let servers connect and communicate with clients
	int	num_ready_fds;
	while (1)
	{
	#ifdef __MACH__
		num_ready_fds = kevent(kqueue_epoll_fd, nullptr, 0, triggered_events, N_EVENTS, nullptr);
	#elif defined(__linux__)
		num_ready_fds = epoll_wait(kqueue_epoll_fd, triggered_events, N_EVENTS, -1);
	#endif

		//debug
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
		std::cout << "\nCluster.run():\n\nkevent()/epoll_wait() returned " << num_ready_fds << " events\n" << std::endl;

		if (num_ready_fds == -1)
		{
			//TODO handle error
			perror("\nERROR\ncluster.run(): kevent()/epoll_wait()");
			exit(EXIT_FAILURE);
		}

		// handle the fds triggered
		for (int i = 0; i < num_ready_fds; ++i)
		{

		#ifdef __MACH__
			Event *current_event = (Event *)(triggered_events[i].udata);
			current_event->is_hang_up = (triggered_events[i].flags & EV_EOF);
			current_event->is_error = (triggered_events[i].flags & EV_ERROR);
		#elif defined(__linux__)
			Event *current_event = (Event *)(triggered_events[i].data.ptr);
			current_event->is_hang_up = (triggered_events[i].events & EPOLLHUP);
			current_event->is_error = (triggered_events[i].events & EPOLLERR);
		#endif

			DefaultServer *default_server = (DefaultServer *)(current_event->default_server_ptr);

			//debug
			std::cout << "\nloop with index =\t" << i << std::endl << std::endl;
			std::cout << "current_event:\n";
			std::cout << "\tfd =\t\t" << current_event->fd << std::endl;
			std::cout << "\tevents =\t" << current_event->events << std::endl;
			std::cout << "\tis_hang_up =\t" << std::boolalpha << current_event->is_hang_up << std::endl;
			std::cout << "\tdefault_server:" << *(DefaultServer *)current_event->default_server_ptr << std::endl << std::endl;

			//TODO manage the case in which (current_event->is_error == true)
			// if fd == connected_fd : close fd and erase client
			// if fd == listening_fd : boh, maybe just ignore it (?)

			if (current_event->events == WRITE)
			{
				//TODO handle case in which (current_event->is_hang_up == true)
				// close fd and erase client

				// response can be sent to connected_fd
				default_server->sendResponse(current_event);
			}
			else if (current_event->events == READ)
			{
				if (current_event->fd == default_server->getListeningFd())
				{
					// listening_fd ready to accept a new connection from client
					default_server->connectToClient();
				}
				else
				{
					// request can be received from connected_fd
					default_server->receiveRequest(current_event);
				}
			}

			//debug
			std::cout << "\n\n'for loop' with index = " << i << " is over. Maximum index = " << num_ready_fds - 1 << std::endl << std::endl;
		}
	}
}
