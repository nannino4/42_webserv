#include "cluster.hpp"

// utils

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
	if (!config_file.eof())
	{
		std::cerr << "\nERROR\nCluster::fileToString(): reading config_file: config_file.fail() = true" << std::endl;
		exit(EXIT_FAILURE);
	}
	return whole_file;
}
