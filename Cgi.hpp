#pragma once

#include <map>
#include <string>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdio>

#include "Request.hpp"

class Cgi
{
private:
	std::map<std::string, std::string>	_env;
	std::string							new_body;
    std::map<std::string, std::string>	cgi_header;
    std::string							post_body_data;

public:
	Cgi(const Request &request);
	~Cgi();
	std::string run_cgi(std::string const &file_name);
	char **map_to_char();
};
