// https://www.jmarshall.com/easy/http/#whatis

#include "Request.hpp"

// default constructor
Request::Request() : \
	request(), \
	request_pos(0), \
	method(), \
	path(), \
	query(), \
	version(), \
	headers(), \
	body(), \
	is_valid(true), \
	is_complete(false), \
	are_headers_complete(false), \
	location(nullptr)
	{}

// copy constructor
Request::Request(const Request &other) { *this = other; }

// assign operator oveload
Request &Request::operator=(const Request &other)
{
	request = other.getRequest();
	request_pos = other.getRequestPos();
	method = other.getMethod();
	path = other.getPath();
	query = other.getQuery();
	version = other.getVersion();
	headers = other.getHeaders();
	body = other.getBody();
	is_valid = other.isValid();
	is_complete = other.isComplete();
	are_headers_complete = other.areHeadersComplete();
	location = other.location;
	return *this;
}

// destructor
Request::~Request() {}

// getters
const std::string						&Request::getRequest() const { return request; }
const int								&Request::getRequestPos() const { return request_pos; }
const std::string						&Request::getVersion() const { return version; }
const std::string						&Request::getMethod() const { return method; }
const std::string						&Request::getPath() const { return path; }
const std::string						&Request::getQuery() const { return query; }
const std::string						&Request::getBody() const { return body; }
const std::map<std::string,std::string>	&Request::getHeaders() const { return headers; }
const std::string						&Request::getHostname() const { return (headers.find("Host"))->second; }
const bool								&Request::isValid() const { return is_valid; }
const bool								&Request::isComplete() const { return is_complete; }
const bool								&Request::areHeadersComplete() const { return are_headers_complete; }
Location const							*Request::getLocation() const { return location; }

// setters
void	Request::setRequest(std::string const new_request) { request = new_request; }
void	Request::setRequestPos(int const new_request_pos) { request_pos = new_request_pos; }
void	Request::setMethod(std::string const new_method) { method = new_method; }
void	Request::setPath(std::string const new_path) { path = new_path; }
void	Request::setQuery(std::string const new_query) { query = new_query; }
void	Request::setVersion(std::string const new_version) { version = new_version; }
void	Request::addHeader(std::pair<std::string,std::string> const new_header) { headers.insert(new_header); }
void	Request::setBody(std::string const new_body) { body = new_body; }
void	Request::setIsValid(bool const new_is_valid) { is_valid = new_is_valid; }
void	Request::setIsComplete(bool const new_is_complete) { is_complete = new_is_complete; }
void	Request::setAreHeadersComplete(bool const new_are_headers_complete) { are_headers_complete = new_are_headers_complete; }
void	Request::setLocation(Location const *new_location) { location = new_location; }

// operator overloads
std::ostream& operator<<(std::ostream &out, const Request &request)
{
	out << "Request:" << std::endl;
	out << request.request << std::endl;
	return out;
}