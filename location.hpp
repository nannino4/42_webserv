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
	bool						is_redirection;
	std::pair<std::string,int>	redirection;

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

	// output operator overload
	friend std::ostream &operator<<(std::ostream &os, Location const &location);

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