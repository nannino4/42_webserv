#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>		//pair

#include "server.hpp"

class Cluster
{
public:
	// aliases
	typedef std::pair<std::string,unsigned short>	host_port_type;

private:
	// attributes
	std::vector<Server>	servers_v;
	std::map< host_port_type, Server& >	defaultServers_m;

public:
	// constructor
	Cluster(std::string config_file)	//NOTE: if the config file is not valid, then default config file is used
	{
		//TODO parsing config file e initialization of cluster
	}

	// destructor
	~Cluster() {}

	// run
	void run()
	{
		for (std::vector<Server>::iterator it = servers_v.begin(); it != servers_v.end(); ++it)
		{
			it->run();
		}
		while (1)
		{
			// TODO connect and communicate
		}
	}

};
