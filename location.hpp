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
	std::string					root;
	std::vector<std::string>	allowed_methods;

	//directory listing
	bool						autoindex;
	std::string					index;

	//redirection
	bool						isRedir;
	std::pair<std::string,int>	redirection;

public:
	// constructor
	Location(std::string &config_file, int &pos);

	// destructor
	~Location();

	// getters
	std::string const					&getRoot() const;
	bool								isMethodAllowed(std::string method) const;
	bool								isAutoindex() const;
	std::pair<std::string,int> const	&getRedirection() const;

private:
	// setters
	void	addAllowedMethod(std::string method);

	// initialization
	void parseRoot(std::stringstream &stream);
	void parseAllowedMethods(std::stringstream &stream);
	void parseAutoindex(std::stringstream &stream);
	void parseIndex(std::stringstream &stream);
	void parseReturn(std::stringstream &stream);

};