// https://www.jmarshall.com/easy/http/#whatis

#include "Request.hpp"

// default constructor
Request::Request() : is_valid(true) {}

// constructor
Request::Request(const std::string &raw_request) : is_valid(true)
{
	std::stringstream file(raw_request);
	std::string line;
	getline(file, line, '\r');

	std::stringstream firts_line(line);
	firts_line >> method >> path >> version;

	while (getline(file, line))
	{
		std::stringstream sline(line);
		std::string key;
		while (getline(sline, key, ':'))
		{
			std::string value;
			sline >> std::ws;
			if (getline(sline, value))
				headers.insert(std::pair<std::string, std::string>(key, value));
			else
				body = key;
		}
	}
}

// copy constructor
Request::Request(const Request &other) { *this = other; }

// assign operator oveload
Request &Request::operator=(const Request &other)
{
	method = other.getMethod();
	path = other.getPath();
	version = other.getVersion();
	headers = other.getHeaders();
	body = other.getBody();
	is_valid = other.isValid();
	return *this;
}

// destructor
Request::~Request() {}

const std::string						&Request::getVersion() const { return version; }
const std::string						&Request::getMethod() const { return method; }
const std::string						&Request::getPath() const { return path; }
const std::string						&Request::getBody() const { return body; }
const std::map<std::string,std::string>	&Request::getHeaders() const { return headers; }
const bool								&Request::isValid() const { return is_valid; }
const std::string						&Request::getHostname() const { return (headers.find("Host"))->second; }

std::ostream& operator<<(std::ostream & out, const Request& m)
{
	out << "HTTP Request:" << std::endl;
	out << "\tMethod: " << m.method << std::endl;
	out << "\tPath: " << m.path << std::endl;
	out << "\tVersion: " << m.version << std::endl;
	out << "\tHeaders: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = m.headers.begin(); it != m.headers.end(); ++it)
		out << "\t\t" << it->first << ": " << it->second << std::endl;
	out << "\tBody: " << m.body << std::endl;
	return out;
}