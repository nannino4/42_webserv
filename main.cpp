#include "cluster.hpp"
#include <signal.h>

Cluster cluster;

void sigintHandler(int sigint)
{
	cluster.destroy();
	exit(sigint);
}

int main(int argc, char **argv)
{
	struct sigaction act;
	act.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &act, nullptr))
	{
		perror("main(): sigaction()");
		exit(1);
	}

	std::string	config_file = argc < 2 ? DEF_CONF : argv[1];
	cluster = Cluster(config_file);
	cluster.run();
}
