
#include <ctime>
#include <iostream>
#include <unistd.h>

int main()
{
	struct timespec time;

	while (1)
	{
		clock_gettime(CLOCK_BOOTTIME, &time);
		std::cout << "sec =\t" << time.tv_sec << std::endl;
		std::cout << "nsec =\t" << time.tv_nsec << std::endl << std::endl;\
		sleep(1);
	}
}
