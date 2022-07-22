// https://www.jmarshall.com/easy/http/#whatis

#include "response.hpp"
#include "request.hpp"

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

Response::Response(const Request &request)
	: body(), response(), response_pos()
{
	version = request.getVersion();
	if (request.getMethod() == "GET")
	{
		std::string path = request.getPath();
		path.erase(0, 1);
		std::ifstream file(path);
		size_t lenght = 0;
		if (file.is_open())
		{
			std::string line;
			while(getline(file, line))
			{
				lenght += line.length();
				body += line + "\r\n";
			}
			status_code = "200";
			reason_phrase = "OK";
			headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(lenght)));
		}
		else
		{
			file.open("error_pages/404.html");
			if (file.is_open())
			{
				std::string line;
				while(getline(file, line))
				{
					lenght += line.length();
					body += line + "\r\n";
				}
			}
			status_code = "404";
			reason_phrase = "File Not Found";
			headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(lenght)));
		}
	}
	response += version + " " + status_code + " " + reason_phrase + "\r\n";
	std::map<std::string, std::string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	response += "\r\n" + body;
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

std::ostream& operator<<(std::ostream &out, const Response &response)
{
	out << "Response: "<< std::endl << response.response << std::endl;
	return out;
}