#include "cluster.hpp"

// default constructor
Cluster::Cluster(std::string config_file_name)
{

#ifdef __MACH__
	kqueue_epoll_fd = kqueue();
#elif defined(__linux__)
	kqueue_epoll_fd = epoll_create1(0);
#endif
	if (kqueue_epoll_fd == -1)
	{
		perror("\nERROR\ncluster.run(): kqueue()/epoll_create1()");
		exit(EXIT_FAILURE);
	}

	std::ifstream		config_file(config_file_name);
	std::stringstream	stream;
	std::string			whole_file("");
	std::string			directive;
	int					pos = 0;
	int					found_pos;

	// check that config_file_name exists and is valid, otherwise use DEF_CONF instead
	if (!config_file.is_open() && !config_file_name.compare(DEF_CONF))
	{
		perror("\nERROR\nCluster: failed opening the default configuration file");
		exit(EXIT_FAILURE);
	}
	if (!config_file.is_open() && config_file_name.compare(DEF_CONF))
	{
		// //std::cout << "\nWARNING\n\"" << config_file_name << "\" is not a valid configuration file. The default configuration file " << DEF_CONF << " is used instead" << std::endl;
		config_file.open(DEF_CONF);
		if (!config_file.is_open())
		{
			perror("\nERROR\nCluster: failed opening the default configuration file");
			exit(EXIT_FAILURE);
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
			std::cerr << "\nERROR\nCluster::Cluster(): stream reading failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (directive.compare("server"))
		{
			std::cerr << "\nERROR\nCluster::Cluster(): parsing config_file: \"" << directive << "\" is an invalid directive at the current scope" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			directive.clear();
			stream >> directive;
			if (!directive.empty())
			{
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
	if (!stream.eof())
		stream >> std::ws;
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nCluster::Cluster(): found invalid text after the last '}'" << std::endl;
	}
	// // //std::cout << "Cluster initialization has been completed.\n" << *this << std::endl;		//debug
}

// destructor
Cluster::~Cluster()
{
	//debug
	//std::cout << "~Cluster()" << std::endl;

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
	struct timespec time_of_last_timeout_check;
	struct timespec null_timespec;

	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &time_of_last_timeout_check);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &time_of_last_timeout_check);
	#endif
	bzero(&null_timespec, sizeof(null_timespec));

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
		num_ready_fds = kevent(kqueue_epoll_fd, nullptr, 0, triggered_events, N_EVENTS, &null_timespec);
	#elif defined(__linux__)
		num_ready_fds = epoll_wait(kqueue_epoll_fd, triggered_events, N_EVENTS, 0);
	#endif

		if (num_ready_fds == -1)
		{
			perror("\nERROR\ncluster.run(): kevent()/epoll_wait()");
			exit(EXIT_FAILURE);
		}

		// handle the fds triggered
		for (int i = 0; i < num_ready_fds; ++i)
		{

		#ifdef __MACH__
			Event *current_event = (Event *)(triggered_events[i].udata);
			if (!current_event->is_hang_up)
				current_event->is_hang_up = (triggered_events[i].flags & EV_EOF);
			current_event->is_error = (triggered_events[i].flags & EV_ERROR);
		#elif defined(__linux__)
			Event *current_event = (Event *)(triggered_events[i].data.ptr);
			current_event->is_hang_up = (triggered_events[i].events & EPOLLHUP);
			if (!current_event->is_hang_up)
				current_event->is_hang_up = (triggered_events[i].events & EPOLLRDHUP);
			current_event->is_error = (triggered_events[i].events & EPOLLERR);
		#endif

			DefaultServer *default_server = (DefaultServer *)(current_event->default_server_ptr);

			//debug
			//std::cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
			//std::cout << "\nevent " << i + 1 << "/" << num_ready_fds << std::endl << std::endl;
			//std::cout << "current_event:\n";
			// //std::cout << "\tevent.fd =\t\t" << triggered_events[i].ident << std::endl;
			//std::cout << "\tcurrent_event.fd =\t" << current_event->fd << std::endl;
			//std::cout << "\tevents =\t" << current_event->events << std::endl;
			//std::cout << "\tis_hang_up =\t" << std::boolalpha << current_event->is_hang_up << std::endl;
			//std::cout << "\tis_error =\t" << std::boolalpha << current_event->is_error << std::endl;
			//std::cout << "\tserver fd:\t" << ((DefaultServer *)current_event->default_server_ptr)->getListeningFd() << std::endl << std::endl;

			if (current_event->is_error)
			{
				default_server->removeEvent((ConnectedClient*)current_event->owner);
				default_server->disconnectFromClient((ConnectedClient*)current_event->owner);
			}
			// else if (current_event->is_hang_up && (current_event->fd != ((ConnectedClient*)current_event->owner)->response.getCgi().getFromCgiFd())) //debug: this shouldn't be commented
			// {
			// 	default_server->removeEvent((ConnectedClient*)current_event->owner);
			// 	default_server->disconnectFromClient((ConnectedClient*)current_event->owner);
			// }
			else if (current_event->events == WRITE)
			{
				if (current_event->fd == ((ConnectedClient*)current_event->owner)->connected_fd)
				{
					// response can be sent to connected_fd
					default_server->sendResponse(current_event);
				}
				else if (current_event->fd == ((ConnectedClient*)current_event->owner)->response.getCgi().getToCgiFd())
				{
					// write to CGI
					default_server->writeToCgi(current_event);
				}
				else
				{
					//error: disconnect
					default_server->removeEvent((ConnectedClient*)current_event->owner);
					default_server->disconnectFromClient((ConnectedClient*)current_event->owner);
				}
			}
			else if (current_event->is_hang_up && (current_event->fd != ((ConnectedClient*)current_event->owner)->response.getCgi().getFromCgiFd())) //debug
			{
				default_server->removeEvent((ConnectedClient*)current_event->owner);
				default_server->disconnectFromClient((ConnectedClient*)current_event->owner);
			}
			else if (current_event->events == READ)
			{
				if (current_event->fd == default_server->getListeningFd())
				{
					// listening_fd ready to accept a new connection from client
					default_server->connectToClient();
				}
				else if (current_event->fd == ((ConnectedClient*)current_event->owner)->connected_fd)
				{
					// request can be received from connected_fd
					default_server->receiveRequest(current_event);
				}
				else if (current_event->fd == ((ConnectedClient*)current_event->owner)->response.getCgi().getFromCgiFd())
				{
					// read from CGI
					default_server->readFromCgi(current_event);
				}
				else
				{
					//error: disconnect
					default_server->removeEvent((ConnectedClient*)current_event->owner);
					default_server->disconnectFromClient((ConnectedClient*)current_event->owner);
				}
			}
			// //std::cout << "\n||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;

		} //for loop on num_ready_fds

		// close timed out connections & finish reading timed out requests
		if (getTimeDifference(time_of_last_timeout_check) > HOW_OFTEN)
		{
			for (std::map<Cluster::address,DefaultServer&>::iterator it = default_servers.begin(); it != default_servers.end(); ++it)
			{
				it->second.closeTimedOutConnections();
			}
		}

	} //while (1)
}
