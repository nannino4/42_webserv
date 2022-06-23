#include "cluster.hpp"

int main(int argc, char **argv)
{
	std::string	config_file = argc < 2 ? DEF_CONF : argv[1];
	Cluster cluster(config_file);
	cluster.run();
}
