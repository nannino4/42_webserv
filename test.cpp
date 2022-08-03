
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <string.h>

int main()
{
	std::string s("ciao");
	char		buf[10];

	bzero(buf, sizeof(buf));
	buf[1] = ' ';
	buf[2] = 'a';
	buf[3] = ' ';
	buf[4] = 't';
	buf[5] = 'u';
	buf[6] = 't';
	buf[7] = 't';
	buf[8] = 'i';

	for (size_t i = 0; i < 10; i++)
	{
		s.push_back(buf[i]);
	}
	
	std::cout << "s.size = " << s.size() << std::endl;

	for (size_t i = 0; i < s.size(); ++i)
	{
		std::cout << "s." << i << "\t" << s.at(i) << std::endl;
	}
	std::cout << "s = \'" << s << "\'" << std::endl;

}
