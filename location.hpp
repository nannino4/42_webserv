#pragma once

#include <string>
#include <map>
#include <vector>

class Location
{
private:
	// attributes
	std::string					path;
	std::string					root;
	std::vector<std::string>	allowed_methods;
	struct	Redirection
	{
		std::string	location;
		int			status_code;
	}							redirection;
public:
	// constructor
	Location(std::string path, std::string content);

	// destructor
	~Location();

	// getters
	std::string const	&getPath() const;
	std::string const	&getRoot() const;
	bool				isMethodAllowed(std::string method) const;
	Redirection const	&getRedirection() const;

private:
	// setters
	void	addAllowedMethod(std::string method);

	// methods

};