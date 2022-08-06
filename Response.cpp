
#include "Response.hpp"

// default constructor
Response::Response() : \
		version("HTTP/1.1"), \
		status_code(), \
		reason_phrase(), \
		headers(), \
		body(), \
		response(), \
		response_pos(0) \
		{}

// copy constructor
Response::Response(const Response &other) { *this = other; }

// assign operator overload
Response &Response::operator=(const Response &other)
{
	version = other.getVersion();
	status_code = other.getStatusCode();
	reason_phrase = other.getReasonPhrase();
	headers = other.getHeaders();
	body = other.getBody();
	response = other.getResponse();
	response_pos = other.getResponsePos();
	return *this;
}

Response::~Response() {}

// getters
std::string const						&Response::getVersion() const { return version; }
std::string const						&Response::getStatusCode() const { return status_code; }
std::string const						&Response::getReasonPhrase() const { return reason_phrase; }
const std::map<std::string,std::string>	&Response::getHeaders() const { return headers; }
std::string const						&Response::getBody() const { return body; }
std::string const						&Response::getResponse() const { return response; }
int const								&Response::getResponsePos() const { return response_pos; }

// setters
void	Response::setVersion(std::string const &new_version) { version = new_version; }
void	Response::setStatusCode(std::string const &new_status_code) { status_code = new_status_code; }
void	Response::setReasonPhrase(std::string const &new_reason_phrase) { reason_phrase = new_reason_phrase; }
void	Response::addNewHeader(std::pair<std::string,std::string> const &new_header) { headers[new_header.first] = new_header.second; }
void	Response::setBody(std::string const &new_body) { body = new_body; }
void	Response::setResponse(std::string const &new_response) { response = new_response; }
void	Response::setResponsePos(int new_pos) { response_pos = new_pos; }

// other methods

void Response::createResponse()
{
	// add first line
	response += version + " " + status_code + " " + reason_phrase + "\r\n";

	// add headers
	if (!body.empty())
	{
		headers["Content-Length"] = std::to_string(body.size());
	}
	for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		response += it->first + ": " + it->second + "\r\n";
	}

	// add body
	response += "\r\n" + body;
}

// operator overloads
std::ostream& operator<<(std::ostream &out, const Response &response)
{
	out << "Response:"<< std::endl << response.response << std::endl;
	return out;
}
