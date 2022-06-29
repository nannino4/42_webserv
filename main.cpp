#include "cluster.hpp"
#include "Cgi.hpp"

std::string get_working_path()
{
   char temp[4096];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}

int main(int argc, char **argv)
{
	std::string	config_file = argc < 2 ? DEF_CONF : argv[1];
	Cluster cluster(config_file);
	cluster.run();
}
