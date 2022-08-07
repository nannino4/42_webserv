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
#include "event.hpp"

class Cgi
{
private:
	int									to_cgi[2];
	int									from_cgi[2];
	std::map<std::string,std::string>	_env;
	char								**env;
	std::string							new_body;
    std::map<std::string,std::string>	headers;
    std::string							post_data;
	Event								*triggered_event;
	pid_t								pid;

public:
	Cgi();
	Cgi(Cgi const &other);
	~Cgi();

	Cgi &operator=(Cgi const &other);
	
	//getters
	int const								&getToCgiFd() const;
	int const								&getFromCgiFd() const;
	std::map<std::string,std::string> const	&get_Env() const;
	char									**getEnv() const;
	std::string const						&getNewBody() const;
	std::map<std::string,std::string> const	&getHeaders() const;
	std::string const						&getPostData() const;
	Event									*getEvent() const;
	pid_t const								&getPid() const;

	//other methods
	void		initialize(Event *event, const Request &request);
	int			run_cgi(std::string const &file_name);
	void		map_to_char();
};
