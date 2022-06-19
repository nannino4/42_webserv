#include "cluster.hpp"

// utils
// open config_file
std::ifstream Cluster::openConfigFile(std::string config_file_name)
{
	std::ifstream config_file(config_file_name);

	// check that config_file_name exists and is valid, otherwise use DEF_CONF instead
	if (!config_file.is_open() && !config_file_name.compare(DEF_CONF))
	{
		//TODO handle error
		perror("\nERROR/nCluster: trying to open default.conf")
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
			perror("\nERROR\nCluster: trying to open default.conf")
			exit(EXIT_FAILURE);
			// throw();
		}
	}
	return config_file;
}

// copy file to string
std::string	Cluster::fileToString(std::ifstream config_file)
{
	std::string string;
	std::string whole_file("");

	while (config_file.good())
	{
		std::getline(config_file, string);
		string += "\n";
		whole_file += string;
	}
	if (config_file.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nCluster::Cluster(): reading config_file: config_file.fail() = true" << std::endl;
		exit(EXIT_FAILURE);
	}
	return whole_file;
}

// initialization
// parse "location" block
void Cluster::parseServerDirectives()
{
	//TODO
}

// parse server directives
void Cluster::parseServerDirectives()
{
	//TODO
}

// parse "server" block
void Cluster::parseServerBlock(std::string &whole_file, int &pos)
{
	std::stringstream	stream;
	std::string			directive;
	int					found_pos;

	// check if file doesn't contain any character among "{;}" - '}' is required to close the "server" block
	if ((found_pos = whole_file.find_first_of("{;}", pos, 3)) == std::string::npos)
	{
		//TODO handle error
		std::cerr << "\nERROR\nCluster::parseServerBlock(): expected '}'" << std::endl;
		exit(EXIT_FAILURE);
	}

	DefaultServer newServer(kqueue_fd, BACKLOG_SIZE);		// still can't know if it's going to be a Server or DefaultServer

	// parse server block searching for directives and 'location' blocks
	while (whole_file[found_pos] != '}')
	{
		if (whole_file[found_pos] == ';')
		{
			parseServerDirectives();	//TODO
		}
		else if (whole_file[found_pos] == '{')
		{
			parseLocationBlock();		//TODO
		}

		if ((found_pos = whole_file.find_first_of("{;}", pos, 3)) == std::string::npos)
		{
			//TODO handle error
			std::cerr << "\nERROR\nCluster::parseServerBlock(): expected '}'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	//check whether there is already a default server with the same address:port combination
	if (default_servers.find(newServer.getAddress()) == default_servers.end())
	{
		// adding new default server
		default_servers[newServer.getAddress()] = newServer;
	}
	else
	{
		// adding new virtual server because there is already a default server with same address:port combination
		(default_servers.find(newServer.getAddress()))->second.addVirtualServer(newServer);
	}
}

// default constructor
Cluster::Cluster(std::string config_file_name)	//NOTE: if the config file is not valid, then default config file is used
{
	std::ifstream config_file = openConfigFile(config_file_name);
	std::stringstream	stream;
	std::string			whole_file("");
	std::string			directive;
	int					pos = 0;
	int					found_pos;

	whole_file = fileToString(config_file);

	// now whole_file contains the whole file
	
	// parse whole_file searching for 'server' blocks
	while ((found_pos = whole_file.find_first_of('{', pos)) != std::string::npos)
	{
		stream.str(whole_file.substr(pos, (found_pos - pos)));
		pos = (found_pos + 1);
		directive.clear();
		stream >> directive;
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
				parseServerBlock(whole_file, pos);
			}
		}
	}
}

// constructor per DEBUG
Cluster::Cluster()
{
	default_servers.insert(std::pair<address,DefaultServer>(address(inet_addr("0.0.0.0"), htons(8080)), DefaultServer(kqueue_fd, BACKLOG_SIZE)));
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
