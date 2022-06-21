#include "default_server.hpp"

// parse directives
void DefaultServer::parseDirectives(std::stringstream &stream)
{
	std::string directive;

	directive.clear();
	stream >> directive;

	if (directive.empty())
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseServerDirectives(): expected directive before ';'" << std::endl;
		exit(EXIT_FAILURE);
	}

	// if stream reached EOF we can leave the default settings
	if (stream.eof())
		return ;
	
	// parse directive
	if (!directive.compare("listen"))
	{
		parseListen(stream);
	}
	else if (!directive.compare("server_name"))
	{
		parseName(stream);			//TODO
	}
	else if (!directive.compare("limit_body_size"))
	{
		parseBodySize(stream);		//TODO
	}
	else if (!directive.compare("error_page"))
	{
		parseErrorPage(stream);		//TODO
	}
	else
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseServerDirectives(): \"" << directive << "\" is an invalid directive" << std::endl;
		exit(EXIT_FAILURE);
	}
}

// parse "listen" directive
void DefaultServer::parseListen(std::stringstream &stream)
{
	int			found_pos;
	std::string	string;
	std::string address;
	std::string port;

	stream >> string;
	stream >> std::ws;

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseListen(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}

	if ((found_pos = string.find_first_of(':')) != std::string::npos)
	{
		// address:port
		address = string.substr(0, found_pos);
		port = string.substr(found_pos + 1);
	}
	else if ((found_pos = string.find_first_of('.')) != std::string::npos)
	{
		// only address
		address = string;
	}
	else if (!string.empty())
	{
		// only port
		port = string;
	}

	// initialize server address
	if (!address.empty())
	{
		server_addr.sin_addr.s_addr = inet_addr(address);
		if (server_addr.sin_addr.s_addr == INADDR_NONE)
		{
			//TODO handle error
			std::cerr << "\nERROR\nDefaultServer::parseListen(): inet_addr() returned an invalid ip address" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!port.empty())
	{
		if (port.find_first_not_of("0123456789", 0, 10) != std::string::npos)
		{
			//TODO handle error
			std::cerr << "\nERROR\nDefaultServer::parseListen(): \"" << port << "\" is an invalid port" << std::endl;
			exit(EXIT_FAILURE);
		}
		int port_n = atoi(port);
		if (port_n < 1 || port_n > 65535)
		{
			//TODO handle error
			std::cerr << "\nERROR\nDefaultServer::parseListen(): \"" << port_n << "\" is an invalid port" << std::endl;
			exit(EXIT_FAILURE);
		}
		server_addr.sin_port = htons(port_n);
	}
}

// parse "server_name" directive
void DefaultServer::parseName(std::stringstream &stream)
{
	std::string	newName;

	while (stream.good())
	{
		stream >> newName;
		stream >> std::ws;
		names.push_back(newName);
	}
	
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseName(): failed reading from stream" << std::endl;
	}
}

// parse "limit_body_size" directive
void DefaultServer::parseBodySize(std::stringstream &stream)
{
	std::string bodySize;

	stream >> bodySize;
	stream >> std::ws;

	// check that bodySize is a number
	if (port.find_first_not_of("0123456789", 0, 10) != std::string::npos)
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseBodySize(): \"" << bodySize << "\" is an invalid body size" << std::endl;
		exit(EXIT_FAILURE);
	}

	client_body_size = atoi(bodySize);

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseBodySize(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}
}

// parse "error_page" directive
void DefaultServer::parseErrorPage(std::stringstream &stream)
{
	int			status_code;
	std::string	path;

	stream >> code >> path;
	stream >> std::ws;

	// check that stream reached EOF
	if (!stream.eof())
	{
		//TODO handle error
		std::cerr << "\nERROR\nDefaultServer::parseErrorPage(): too many parameters" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!path.empty())
	{
		std::ifstream file(path);
		if (!file.good())
		{
			//TODO handle error
			std::cerr << "\nERROR\nDefaultServer::parseErrorPage(): failed opening \"" << path << "\"" << std::endl;
			exit(EXIT_FAILURE);
		}
		file.close();
		error_pages[status_code] = path;
	}
}
