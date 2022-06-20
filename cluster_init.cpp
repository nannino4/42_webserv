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
