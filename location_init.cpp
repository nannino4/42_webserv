#include "location.hpp"

void Location::parseRoot(std::stringstream &stream)
{
	stream >> root;
	stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseRoot(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseRoot(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseAllowedMethods(std::stringstream &stream)
{
	std::string	newMethod;

	while (stream.good())
	{
		stream >> newMethod;
		stream >> std::ws;

		// check that stream didn't fail reading
		if (stream.fail())
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::parseAllowedMethods(): stream reading failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		// check that newMethod is valid
		if (newMethod.compare("GET") && newMethod.compare("POST") && newMethod.compare("DELETE"))
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::parseAllowedMethods(): \"" << newMethod << "\" is an invalid method" << std::endl;
		}

		allowed_methods.push_back(newMethod);
	}
	
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseAllowedMethods(): failed reading from stream" << std::endl;
	}
}

void Location::parseAutoindex(std::stringstream &stream)
{
	std::string flag;

	stream >> flag;
	stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseAutoindex(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseAutoindex(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that flag is either "on" or "off" and set autoindex correspondingly
	if (!flag.compare("on"))
	{
		autoindex = true;
	}
	else if (!flag.compare("off"))
	{
		autoindex = false;
	}
	else
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseAutoindex(): \"" << flag << "\" is an invalid flag. Expected \"on\" or \"off\"" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseIndex(std::stringstream &stream)
{
	stream >> index;
	stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseIndex(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseIndex(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseReturn(std::stringstream &stream)
{
	int			code;
	std::string	url;

	isRedir = true;

	stream >> code;
	stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseReturn(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream hasn't reached EOF
	if (stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseReturn(): only one paramenter, expected two" << std::endl;
		exit(EXIT_FAILURE);
	}

	stream >> url;
	stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseReturn(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseReturn(): too many parameters, expected two" << std::endl;
		exit(EXIT_FAILURE);
	}

	redirection.first = url;
	redirection.second = code;
}
