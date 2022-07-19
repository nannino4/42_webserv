#include "location.hpp"

// constructor
Location::Location(std::string &config_file, int &pos) : autoindex(false), index("index.html"), isRedir(false)
{
	std::stringstream	stream;
	std::string			directive;
	int					found_pos;

	// check if file doesn't contain any character among ";}" - '}' is required to close the "server" block
	if ((unsigned long)(found_pos = config_file.find_first_of(";}", pos, 2)) == std::string::npos)
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::Location(): expected '}'" << std::endl;
		exit(EXIT_FAILURE);
	}

	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");

	// parse location block searching for directives
	while (config_file[found_pos] != '}')
	{
		stream.clear();
		stream.str(config_file.substr(pos, (found_pos - pos)));
		pos = found_pos + 1;
	
		stream >> directive;
		stream >> std::ws;

		// check that stream didn't fail reading
		if (stream.fail())
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::Location(): stream reading failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		// check that directory exists
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
			parseAllowedMethods(stream);
		}
		else if (!directive.compare("directory_listing"))
		{
			parseAutoindex(stream);
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

		if ((unsigned long)(found_pos = config_file.find_first_of(";}", pos, 3)) == std::string::npos)
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

// operator overload
std::ostream &operator<<(std::ostream &os, Location const &location)
{
	os << "\nLocation introducing itself:\n";
	os << "root:\n" << location.root << std::endl;
	os << "allowed_methods:\n";
	for (std::vector<std::string>::const_iterator it = location.allowed_methods.begin(); it != location.allowed_methods.end(); ++it)
	{
		os << *it << std::endl;
	}
	os << "autoindex:\n" << std::boolalpha << location.autoindex << std::endl;
	os << "index:\n" << location.index << std::endl;
	os << "isRedir:\n" << std::boolalpha << location.isRedir << std::endl;
	os << "redirection:\n" << location.redirection.second << " " << location.redirection.first << std::endl;
	os << "\nLocation introduction is over" << std::endl;
	return os;
}

// getters
std::string const					&Location::getRoot() const { return root; }
bool 								Location::isAutoindex() const { return autoindex; }
std::pair<std::string,int> const	&Location::getRedirection() const { return redirection; }
bool 								Location::isMethodAllowed(std::string method) const
{
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (!it->compare(method.c_str()))
			return true;
	}
	return false;
}

// setters
void	Location::addAllowedMethod(std::string method)
{
	if (!isMethodAllowed(method))
		allowed_methods.push_back(method);
}
