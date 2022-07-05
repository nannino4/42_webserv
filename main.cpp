#include "cluster.hpp"
//#include "Cgi.hpp"

/* std::string get_working_path()
{
   char temp[4096];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
} */

int main()
{
	// std::string	config_file = argc < 2 ? std::string() : argv[1];
	// Cluster cluster(config_file);	//NOTE: if the config file is not valid, then default config file is used
	Cluster cluster;
	cluster.run();
	//Cgi cgi;
	//cgi.run_cgi(get_working_path() + "/pages/index.php");
	
}
