
#include <iostream>
#include <unistd.h>
#include "utils.hpp"
#include <string>

int main()
{
	char *tmp = get_current_dir_name();
	std::string string(tmp);
	delete tmp;
	std::cout << string << std::endl;
}
