#include "cluster.hpp"

int main()
{
	// std::string	config_file = argc < 2 ? std::string() : argv[1];
	// Cluster cluster(config_file);	//NOTE: if the config file is not valid, then default config file is used
	Cluster cluster;
	cluster.run();
}
