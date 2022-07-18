// https://www.jmarshall.com/easy/http/#whatis

#include "request.hpp"

// default constructor
Request::Request() : is_valid(false) {}

// copy constructor
Request::Request(const Request &other) { *this = other; }

// assign operator oveload
Request &Request::operator=(const Request &other)
{
	message = other.getMessage();
	message_pos = other.getMessagePos();
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

// getters
const std::string						&Request::getMessage() const { return message; }
const int								&Request::getMessagePos() const { return message_pos; }
const std::string						&Request::getVersion() const { return version; }
const std::string						&Request::getMethod() const { return method; }
const std::string						&Request::getPath() const { return path; }
const std::string						&Request::getBody() const { return body; }
const std::map<std::string,std::string>	&Request::getHeaders() const { return headers; }
const std::string						&Request::getHostname() const { return (headers.find("Host"))->second; }
const bool								&Request::isValid() const { return is_valid; }
bool									Request::areHeadersComplete() const
{
	bool ret = (message.find("\r\n\r\n") != std::string::npos) | (message.find("\n\n") != std::string::npos);
	return ret;
}

// setters
void	Request::setMessage(std::string new_message) { message = new_message; }
void	Request::setMessagePos(int new_message_pos) { message_pos = new_message_pos; }

// operator overloads
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