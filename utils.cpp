
#include "utils.hpp"

int	getTimeDifference(struct timespec time_since_last_check)
{
	struct timespec current_time;
	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &current_time);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &current_time);
	#endif

	int then = time_since_last_check.tv_sec;
	int now = current_time.tv_sec;

	return (now - then);
}

std::string	my_getcwd()
{
	#ifdef __MACH__
		char temp[4096];
		return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
	#elif defined(__linux__)
		char *tmp = get_current_dir_name();
		std::string string(tmp);
		delete tmp;
		return std::string(tmp);
	#endif
}
