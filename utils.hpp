#pragma once

#include <ctime>
#include <string>
#include <unistd.h>

int			getTimeDifference(struct timespec time_since_last_check);
std::string	my_getcwd();
