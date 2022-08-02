
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>

int main()
{
	std::ifstream file("./test.txt");
	std::stringstream sstream;

	sstream << file.rdbuf();

	std::cout << "sstream.str():" << std::endl;
	std::cout << "\"" << sstream.str() << "\"" << std::endl;
	std::cout << "\nsstream.str().size():" << sstream.str().size() << std::endl;

}
