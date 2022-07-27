
#include "utils.hpp"

int	getTimeDifference(struct timespec time_since_last_check)
{
	struct timespec current_time;
	clock_gettime(CLOCK_BOOTTIME, &current_time);

	int then = time_since_last_check.tv_sec;
	int now = current_time.tv_sec;

	return (now - then);
}

std::string	my_getcwd()
{
	char *tmp = get_current_dir_name();
	std::string string(tmp);
	delete tmp;
	return std::string(tmp);
}
