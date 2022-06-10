#include "cluster.hpp"

// Cluster::Cluster(std::string config_file_name)	//NOTE: if the config file is not valid, then default config file is used
// {
	// std::ifstream config_file(config_file_name);

	// // check that config_file_name exists and is valid, otherwise use DEF_CONF instead
	// if (!config_file.is_open() && !config_file_name.compare(DEF_CONF))
	// {
	// 	//TODO handle error
	// 	// perror("ERROR/nCluster: trying to open default.conf")
	// 	// throw();
	// }
	// if (!config_file.is_open() && config_file_name.compare(DEF_CONF))
	// {
	// 	std::cout << "WARNING\n" << config_file << " is not a valid configuration file. The default configuration file " << DEF_CONF << " is used instead" << std::endl;
	// 	config_file.open(DEF_CONF);
	// 	if (!config_file.is_open())
	// 	{
	// 		//TODO handle error
	// 		// perror("ERROR\nCluster: trying to open default.conf")
	// 		// throw();
	// 	}
	// }
	// // parse config_file in search for 'server' directives to create servers
	// std::stringstream	line;
	// std::string			tmp;
	// std::string			directive;

	// std::getline(config_file, tmp);
	// while (config_file.good())
	// {
	// 	if (!tmp.empty())
	// 	{
	// 		line.str(tmp);
	// 		line >> directive;
	// 		if (!directive.compare("server"))
	// 		{
	// 			line >> tmp;
	// 			if (tmp.compare("{"))
	// 			{
	// 				//TODO handle error
	// 				// std::cerr << "ERROR\nCluster: parsing config_file: The server directive should only be followed by space and \"{\"" << std::endl;
	// 				// throw()
	// 			}
	// 			else
	// 			{
	// 				line >> tmp;
	// 				if (tmp.empty())
	// 				{
	// 					servers.push_back(Server(*this, BACKLOG_SIZE, config_file));
	// 				}
	// 				else
	// 				{
	// 					//TODO handle error
	// 					// std::cerr << "ERROR\nCluster: parsing config_file: The server directive should only be followed by space and \"{\"" << std::endl;
	// 					// throw()
	// 				}
	// 			}
	// 		}
	// 		else
	// 		{
	// 			//TODO handle error
	// 			// std::cerr << "ERROR\nCluster: parsing config_file: directive " << directive << " is invalid" << std::endl;
	// 			// throw()
	// 		}
	// 	}
	// 	std::getline(config_file, tmp);
	// }
// }

// constructor per DEBUG
Cluster::Cluster()
{
	default_servers.insert(std::pair<address,Server>(address("0.0.0.0", 8080), Server(*this, BACKLOG_SIZE)));
}

// destructor
Cluster::~Cluster() {}

// getters
int Cluster::getKqueueFd() const { return kqueue_fd; }

// run
void Cluster::run()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
	{
		//TODO handle error
		// perror("ERROR\ncluster.run(): epoll_create1")
	}
	// make servers listen and add them to kqueue
	for (std::map<address,Server>::iterator it = default_servers.begin(); it != default_servers.end(); ++it)
	{
		it->second.startListening();
		EV_SET(&event, it->second.getListeningFd(), EVFILT_READ, EV_ADD, 0, 0, (void *)&(it->second));
		if (kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1)
		{
			//TODO handle error
			// perror("ERROR\ncluster.run(): adding listeningFd to kqueue with kevent()")
		}
	}
	// let servers connect and communicate with clients
	int	num_ready_fds;
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
