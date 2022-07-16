//____dbalducc___test___for___CGI//
#include "utility.hpp"
#include <fstream>

bool check_file_exist(std::string full_path){
	std::ifstream file(full_path);
	if(file.is_open())
	{
		file.close();
		return(true);
	}
	return(false);
}

std::string get_working_path()
{
   char temp[4096];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}
//______________________________//