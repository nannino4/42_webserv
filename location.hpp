#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

class Location
{
private:
	// attributes
	std::string							root;
	std::vector<std::string>			allowed_methods;
	//directory listing
	bool								autoindex;
	std::string							index;
	//redirection
	bool								is_redirection;
	std::pair<std::string,int>			redirection;
	//cgi
	bool								is_cgi;
	std::string							cgi_script;
	std::map<std::string,std::string>	cgi_params;

public:
	// default constructor
	Location();
	// constructor
	Location(std::string &config_file, int &pos);
	// copy constructor
	Location(Location const &other);
	// assign operator overload
	Location &operator=(Location const &other);
	// destructor
	~Location();

	// getters
	std::string const					&getRoot() const;
	std::vector<std::string> const		&getAllowedMethods() const;
	bool								isMethodAllowed(std::string method) const;
	bool								isAutoindex() const;
	std::string const					&getIndex() const;
	bool								isRedirection() const;
	std::pair<std::string,int> const	&getRedirection() const;

	// setters
	void	setRoot(std::string const &new_root);
	void	addAllowedMethod(std::string method);

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, Location const &location);

private:
	// initialization
	void parseRoot(std::stringstream &stream);
	void parseAllowedMethods(std::stringstream &stream);
	void parseAutoindex(std::stringstream &stream);
	void parseIndex(std::stringstream &stream);
	void parseReturn(std::stringstream &stream);
	void parsePhpCgi(std::stringstream &stream);
	void parsePhpCgiParam(std::stringstream &stream);

};