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
	bool						directory_listing;
	std::string					index;
	struct	Redirection
	{
		std::string	path;
		int	status_code;
	}							redirection;

public:
	// constructor
	Location(std::string &config_file, int &pos);

	// destructor
	~Location();

	// getters
	std::string const	&getRoot() const;
	bool				isMethodAllowed(std::string method) const;
	Redirection const	&getRedirection() const;

private:
	// setters
	void	addAllowedMethod(std::string method);

	// initialization
	void parseRoot(std::stringstream &stream);
	void parseAllowedMethod(std::stringstream &stream);
	void parseDirectoryListing(std::stringstream &stream);
	void parseIndex(std::stringstream &stream);
	void parseReturn(std::stringstream &stream);

};