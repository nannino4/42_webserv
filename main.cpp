#include <iostream>

#include "cluster.hpp"

int main(int argc, char **argv)
{
	while (1)
	{
		std::string	config_file = argc < 2 ? std::string() : argv[1];
		Cluster cluster(config_file);	//NOTE: if the config file is not valid, then default config file is used
		try
		{
			cluster.run();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
