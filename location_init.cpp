#include "location.hpp"

void Location::parseRoot(std::stringstream &stream)
{
	stream >> root;

	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseRoot(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseRoot(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseUploadPath(std::stringstream &stream)
{
	stream >> upload;

	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseUploadPath(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseUploadPath(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseAllowedMethods(std::stringstream &stream)
{
	std::string	newMethod;

	allowed_methods.clear();
	while (stream.good())
	{
		stream >> newMethod;

		// check that stream didn't fail reading
		if (stream.fail())
		{
			std::cerr << "\nERROR\nLocation::parseAllowedMethods(): stream reading failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		// check that newMethod is valid
		if (newMethod.compare("GET") && newMethod.compare("POST") && newMethod.compare("DELETE"))
		{
			std::cerr << "\nERROR\nLocation::parseAllowedMethods(): \"" << newMethod << "\" is an invalid method" << std::endl;
		}

		addAllowedMethod(newMethod);
		stream >> std::ws;
	}
}

void Location::parseAutoindex(std::stringstream &stream)
{
	std::string flag;

	stream >> flag;
	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseAutoindex(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
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
		std::cerr << "\nERROR\nLocation::parseAutoindex(): \"" << flag << "\" is an invalid flag. Expected \"on\" or \"off\"" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseIndex(std::stringstream &stream)
{
	stream >> index;
	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseIndex(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseIndex(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Location::parseReturn(std::stringstream &stream)
{
	int			code;
	std::string	url;

	is_redirection = true;

	stream >> code;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseReturn(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream hasn't reached EOF
	if (stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseReturn(): only one paramenter, expected two" << std::endl;
		exit(EXIT_FAILURE);
	}

	stream >> url;
	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseReturn(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseReturn(): too many parameters, expected two" << std::endl;
		exit(EXIT_FAILURE);
	}

	redirection.first = url;
	redirection.second = code;
}

void Location::parseCgi(std::stringstream &stream)
{
	std::string	extension;
	std::string	script_path;

	stream >> extension >> script_path;

	if (!stream.eof())
		stream >> std::ws;

	// check that stream didn't fail reading
	if (stream.fail())
	{
		std::cerr << "\nERROR\nLocation::parseCGI(): stream reading failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// check that stream reached EOF
	if (!stream.eof())
	{
		std::cerr << "\nERROR\nLocation::parseCGI(): too many parameters, expected two" << std::endl;
		exit(EXIT_FAILURE);
	}

	cgi.insert(std::pair<std::string,std::string>(extension, script_path));
}
