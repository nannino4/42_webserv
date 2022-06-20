#include "location.hpp"

// constructor
Location::Location(std::string &config_file, int &pos) : directory_listing(false)
{
	std::stringstream	stream;
	std::string			directive;
	int					found_pos;

	// check if file doesn't contain any character among ";}" - '}' is required to close the "server" block
	if ((found_pos = config_file.find_first_of(";}", pos, 2)) == std::string::npos)
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::Location(): expected '}'" << std::endl;
		exit(EXIT_FAILURE);
	}

	// parse location block searching for directives
	while (config_file[found_pos] != '}')
	{
		stream.str(config_file.substr(pos, (found_pos - pos)));
		pos = found_pos + 1;
	
		stream >> directive;

		if (directive.empty())
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::Location(): expected directive before ';'" << std::endl;
			exit(EXIT_FAILURE);
		}

		// if stream reached EOF we can leave the default settings
		if (stream.eof())
			return ;

		// parse directives
		if (!directive.compare("root"))
		{
			parseRoot(stream);
		}
		else if (!directive.compare("allowed_method"))
		{
			parseAllowedMethod(stream);
		}
		else if (!directive.compare("directory_listing"))
		{
			parseDirectoryListing(stream);
		}
		else if (!directive.compare("index"))
		{
			parseIndex(stream);
		}
		else if (!directive.compare("return"))
		{
			parseReturn(stream);
		}
		else
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::Location(): \"" << directive << "\" is an invalid directive" << std::endl;
			exit(EXIT_FAILURE);
		}

		if ((found_pos = config_file.find_first_of(";}", pos, 3)) == std::string::npos)
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::Location(): expected '}'" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// set pos after found_pos - which is pointing to '}'
	pos = found_pos + 1;
}

// destructor
Location::~Location() {}

// getters
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
