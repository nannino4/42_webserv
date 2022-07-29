#pragma once

#include <ctime>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <locale>

int			getTimeDifference(struct timespec time_since_last_check);
std::string	my_getcwd();
std::string date();
std::string last_modified(std::string file);
