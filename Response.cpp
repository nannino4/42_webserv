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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Response::Response(const Request & r, Server &other)
	: body(), response(), srv(other), request(r)
{
	version = request.getVersion();
	if (request.getMethod() == "GET")
		checkMethod("GET", &Response::get);
	if (request.getMethod() == "DELETE")
		checkMethod("DELETE", &Response::delet);
	response += version + " " + response_status_code + " " + reason_phrase + "\r\n";
	headers["Content-Length"] = std::to_string(body.size());
	std::unordered_map<std::string, std::string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	response += "\r\n" + body;
}

void Response::checkMethod(std::string method, void (Response::*f)())
{
	std::map<std::string,Location>::const_iterator it;
	if ((it = srv.getLocations().find(request.getPath().substr(1, request.getPath().find_last_of('/')))) != srv.getLocations().end()
		&& !it->second.isMethodAllowed(method))
	{
		response_status_code = "405";
		reason_phrase = "Method Not Allowed";
		generateErrorPage();
	}
	else
		(this->*f)();
}

void Response::delet()
{
	std::ifstream file;
	struct stat buf;

	stat(("." + request.getPath()).c_str(), &buf);
	if (S_ISREG(buf.st_mode))
	{
		unlink(("." + request.getPath()).c_str());
		response_status_code = "200";
		reason_phrase = "OK";
	}
	else
	{
		response_status_code = "404";
		reason_phrase = "File Not Found";
		generateErrorPage();
	}
}

void Response::get()
{
	size_t pos = request.getPath().find(".php");
	if(pos < request.getPath().size())
	{
		std::cout << "executing cgi file: " << pos << "\n";
		Cgi cgi(request);
		body += cgi.run_cgi("./cgi_tester");
		response_status_code = "200";
		reason_phrase = "OK";
		headers["Content-Length"] = std::to_string(body.size());
		return;
	}
	std::ifstream file;
	struct stat buf;
	std::stringstream line;

	stat(("." + request.getPath()).c_str(), &buf);
	if (S_ISDIR(buf.st_mode))
		manageDir();
	else if (S_ISREG(buf.st_mode))
		fileTobody(("." + request.getPath()));
	else
	{
		response_status_code = "404";
		reason_phrase = "File Not Found";
		generateErrorPage();
	}
}

void Response::fileTobody(std::string const & index)
{
	std::ifstream file;
	std::stringstream line;

	file.open(index);
	if (file.is_open())
	{
		line << file.rdbuf();
		body = line.str();
		response_status_code = "200";
		reason_phrase = "OK";
	}
	else if (file.fail())
	{
		response_status_code = "403";
		reason_phrase = "Forbidden";
		generateErrorPage();
	}
}

void Response::manageDir()
{
	std::map<std::string,Location>::const_iterator it = srv.getLocations().find(request.getPath());
	if (it == srv.getLocations().end())
	{
		if ((it = srv.getLocations().find(request.getPath().substr(1, request.getPath().size()))) == srv.getLocations().end())
			return ;
	}
	if (!it->second.getRoot().empty())
	{
		request.setPath(it->second.getRoot());
		manageDir();
	}
	else if (!it->second.getRedirection().first.empty())
	{
		response_status_code = std::to_string(it->second.getRedirection().second);
		reason_phrase = "Moved Permanently";
		headers["Location"] = it->second.getRedirection().first;
		generateAutoIndex();
	}
	else if (it->second.isAutoindex())
		generateAutoIndex();
	else if (!it->second.getIndex().empty())
		fileTobody("./" + it->first + it->second.getIndex());
	else
		std::cout << "nothing to do with " << request.getPath();
}

void Response::generateErrorPage()
{
	std::map<int, std::string>::const_iterator it;
	std::stringstream s_to_int;
	int code;

	s_to_int << response_status_code;
	s_to_int >> code;
	
	// Managing Declared Error Page
	if ((it = srv.getErrorPages().find(code)) != srv.getErrorPages().end())
	{
		std::ifstream file(it->second);
		if (file.is_open())
		{
			std::string line;
			while(getline(file, line))
				body += line + "\n";
			return ;
		}
	}

	// AutoGenerating an Error Page 
	std::stringstream line;
	line << "<html><head><title>Error "<< response_status_code <<" - " << reason_phrase << "</title>";
	line << "<link rel=\"stylesheet\" href=\"/pages/base.css\"";
	line << "<meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	line << "</head><body><main><center><br /><br /><br /><br /><br /><br/>";
	line << "<h1>Autogenerated Error Page</h1>";
	line << "<h2>Error: " << response_status_code << "</h1>";
	line << "<h2>" << reason_phrase << "</h3>";
	line << "<br /><br /><br /><br /></center></main></body></html>";
	body = line.str();
}


void Response::generateAutoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::stringstream line;

	if ((dir = opendir(("./" + request.getPath()).c_str())) != NULL)
	{
		line << "<html><head><title>Index of " << request.getPath() << reason_phrase << "</title>";
		line << "<link rel=\"stylesheet\" href=\"/pages/base.css\"";
		line << "<meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
		line << "</head><body><main>";
		
		line << "<h1>Index of " << request.getPath() << "</h1>\n";
		line << "<table>";
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			if (std::string(ent->d_name) != ".")
			{
				line << "<tr><td><a href=\"" << ent->d_name << "\">";
				line << ent->d_name << "</a></td></tr>";
			}
		}
		line << "</table></main></body></html>";
		body = line.str();
		closedir (dir);
	}
	else
	{
		/* could not open directory */
		perror ("could not open directory");
		response_status_code = "404";
		reason_phrase = "File Not Found";
		generateErrorPage();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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