#pragma once

#include <map>
#include <string>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#ifdef __linux__
#include <sys/wait.h>
#endif

#include "utility.hpp"


class Cgi
{
private:
	std::map<std::string, std::string> _env; // execve accetta mappa?/// pescare conf && request
	std::string new_body;
public:
	Cgi();
	~Cgi();
	const std::string & getNew_body() const;
	std::string run_cgi(std::string script_name);
	char **map_to_char();//std::map<std::string, std::string> _env);
	void get_env(void);
};