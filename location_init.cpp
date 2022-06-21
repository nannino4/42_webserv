#include "location.hpp"

void Location::parseRoot(std::stringstream &stream)
{
	stream >> root;
	stream >> std::ws;

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseRoot(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseAllowedMethod(std::stringstream &stream)
{
	std::string	newMethod;

	while (stream.good())
	{
		stream >> newMethod;
		stream >> std::ws;
		if (newMethod.compare("GET") && newMethod.compare("POST") && newMethod.compare("DELETE"))
		{
			//TODO handle error
			std::cerr << "\nERROR\nLocation::parseAllowedMethod(): \"" << newMethod << "\" is an invalid method" << std::endl;
		}
		allowed_methods.push_back(newMethod);
	}
	
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nLocation::parseAllowedMethod(): failed reading from stream" << std::endl;
	}
}

void Location::parseAutoindex(std::stringstream &stream)
{
	std::string flag;

	stream >> flag;
	stream >> std::ws;

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
	//TODO
}
