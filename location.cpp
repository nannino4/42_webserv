#include "location.hpp"

// default constructor
Location::Location()
{
	//TODO
	// root = current working directory
	allowed_methods.push_back("GET");
	autoindex = false;
	index.clear();
	is_redirection = false;
}

// constructor
Location::Location(std::string &config_file, int &pos) : autoindex(false), index("index.html"), is_redirection(false)
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

// copy constructor
Location::Location(Location const &other) { *this = other; }

// assign operator overload
Location &Location::operator=(Location const &other)
{
	root = other.getRoot();
	allowed_methods = other.getAllowedMethods();
	autoindex = other.isAutoindex();
	index = other.getIndex();
	is_redirection = other.isRedirection();
	redirection = other.getRedirection();
}

// destructor
Location::~Location() {}

// getters
std::string const					&Location::getRoot() const { return root; }
std::vector<std::string> const		&Location::getAllowedMethods() const { return allowed_methods; }
bool 								Location::isMethodAllowed(std::string method) const
{
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (!it->compare(method))
			return true;
	}
	return false;
}
bool 								Location::isAutoindex() const { return autoindex; }
std::string const					&Location::getIndex() const { return index; }
bool								Location::isRedirection() const { return is_redirection; }
std::pair<std::string,int> const	&Location::getRedirection() const { return redirection; }

// setters
void	Location::addAllowedMethod(std::string method)
{
	if (!isMethodAllowed(method))
		allowed_methods.push_back(method);
}

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
