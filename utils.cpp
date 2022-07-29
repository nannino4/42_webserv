
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

std::string date()
{
	std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %X %Z", std::localtime(&t));
	return std::string(mbstr);
}

std::string last_modified(std::string file)
{
	struct stat f;
	char mbstr[100];

	stat(file.c_str(), &f);
	strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %X %Z", std::localtime(&f.st_mtime));
	return std::string(mbstr);
}

std::string	my_getcwd()
{
	char temp[4096];
	return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}
