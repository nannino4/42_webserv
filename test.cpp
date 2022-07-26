
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
	struct stat	buf;
	std::cout << stat("./testone.cpp", &buf) << std::endl;
}
