#include "location.hpp"

// constructor
Location::Location(std::string path, std::string content) :
path(path)
{
	//TODO parse content and initialize location
}

// destructor
Location::~Location() {}

// getters
std::string const &Location::getPath() const { return path; }
std::string const &Location::getRoot() const { return root; }
bool Location::isMethodAllowed(std::string method) const
{
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (!it->compare(method.c_str()))
			return true;
	}
	return false;
}
Location::Redirection const	&Location::getRedirection() const { return redirection; }

// setters
void	Location::addAllowedMethod(std::string method)
{
	if (!isMethodAllowed(method))
		allowed_methods.push_back(method);
}

// methods
