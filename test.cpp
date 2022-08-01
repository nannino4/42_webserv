
#include <iostream>
#include <unistd.h>
#include <string>

int main()
{
	std::string path("/ciao/boh");
	std::string	directory_path = path.substr(0, path.find_last_of('/'));
	std::string	file_path = path.substr(path.find_last_of('/'));

	std::cout << "directory path =\t" << directory_path << std::endl;
	std::cout << "file path =\t\t" << file_path << std::endl;
}
