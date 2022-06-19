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
		perror("\nERROR/nCluster: trying to open default.conf");
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
			perror("\nERROR\nCluster: trying to open default.conf");
			exit(EXIT_FAILURE);
			// throw();
		}
	}
	return config_file;
}

// copy file to string
std::string	Cluster::fileToString(std::ifstream &config_file)
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

// parse "server" block
void Cluster::parseServerBlock(std::string &whole_file, int &pos)
{
	std::stringstream	stream;
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
		stream.str(whole_file.substr(pos, (found_pos - pos)));
		pos = found_pos + 1;
	
		if (whole_file[found_pos] == ';')
		{
			parseServerDirectives(newServer, stream);
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

// parse server directives
void Cluster::parseServerDirectives(DefaultServer &newServer, std::stringstream &stream)
{
	std::string directive;

	directive.clear();
	stream >> directive;

	if (directive.empty())
	{
		//TODO handle error
		std::cerr << "\nERROR\nCluster::parseServerDirectives(): expected directive before ';'" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// parse directive
	if (!directive.compare("listen"))
	{
		parseListen(newServer, stream);
	}
	else if (!directive.compare("server_name"))
	{
		parseName(newServer, stream);			//TODO
	}
	else if (!directive.compare("limit_body_size"))
	{
		parseBodySize(newServer, stream);		//TODO
	}
	else if (!directive.compare("error_page"))
	{
		parseErrorPage(newServer, stream);		//TODO
	}
	else
	{
		//TODO handle error
		std::cerr << "\nERROR\nCluster::parseServerDirectives(): \"" << directive << "\" is an invalid directive" << std::endl;
		exit(EXIT_FAILURE);
	}
}

// parse "listen" directive
void Cluster::parseListen(DefaultServer &newServer, std::stringstream &stream)
{
	int			found_pos;
	std::string	string("");
	std::string address("");
	std::string port("");

	stream >> string;
	if ((found_pos = string.find_first_of(':')) != std::string::npos)
	{
		// address:port
		address = string.substr(0, found_pos);
		port = string.substr(found_pos + 1);
	}
	else if ((found_pos = string.find_first_of('.')) != std::string::npos)
	{
		// only address
		address = string;
	}
	else
	{
		// only port
		port = string;
	}

	// initialize server address
	if (!address.empty())
		newServer.setIp(address);
	if (!port.empty())
		newServer.setPort(port);
}

// parse "server_name" directive
void Cluster::parseName(DefaultServer &newServer, std::stringstream &stream)
{
    //TODO
}

// parse "limit_body_size" directive
void Cluster::parseBodySize(DefaultServer &newServer, std::stringstream &stream)
{
    //TODO
}

// parse "error_page" directive
void Cluster::parseErrorPage(DefaultServer &newServer, std::stringstream &stream)
{
    //TODO
}

// parse "location" block
void Cluster::parseLocationBlock()
{
	//TODO
}
