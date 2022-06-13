// https://www.jmarshall.com/easy/http/#whatis

#include "Request.hpp"

Request::Request(const std::string & raw_request)
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
				message = key;
		}
	}
}

Request::~Request() {}

const std::string & Request::getVersion() const { return version; }
const std::string & Request::getMethod() const { return method; }
const std::string & Request::getPath() const { return path; }

std::string Request::getRequestHeaderHost() const
{
	std::string ret;
	for (HeaderConstIterator it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == "Host")
			ret = it->second;
	}
	return ret;
}


std::ostream& operator<<(std::ostream & out, const Request& m)
{
	out << "HTTP Request:" << std::endl;
	out << "\tMethod: " << m.method << std::endl;
	out << "\tPath: " << m.path << std::endl;
	out << "\tVersion: " << m.version << std::endl;
	out << "\tHeaders: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = m.headers.begin(); it != m.headers.end(); ++it)
		out << "\t\t" << it->first << ": " << it->second << std::endl;
	out << "\tMessage: " << m.message << std::endl;
	return out;
}