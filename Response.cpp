// https://www.jmarshall.com/easy/http/#whatis

#include "response.hpp"
#include "request.hpp"

// default constructor
Response::Response() {}

// copy constructor
Response::Response(const Response &other) { *this = other; }

// assign operator overload
Response &Response::operator=(const Response &other)
{
	//TODO
	version = other.getVersion();
	status_code = other.getStatusCode();
	reason_phrase = other.getReasonPhrase();
	response = other.getResponse();
	response_pos = other.getResponsePos();
	return *this;
}

Response::Response(const Request &request)
	: message(), response(), response_pos()
{
	version = request.getVersion();
	if (request.getMethod() == "GET")
	{
		std::string path = request.getPath();
		path.erase(0, 1);
		ifstream file(path);
		size_t lenght = 0;
		if (file.is_open())
		{
			string line;
			while(getline(file, line))
			{
				lenght += line.length();
				message += line + "\r\n";
			}
			status_code = "200";
			reason_phrase = "OK";
			headers.insert(pair<string, string>("Content-Length", to_string(lenght)));
		}
		else
		{
			file.open("error_pages/404.html");
			if (file.is_open())
			{
				string line;
				while(getline(file, line))
				{
					lenght += line.length();
					message += line + "\r\n";
				}
			}
			status_code = "404";
			reason_phrase = "File Not Found";
			headers.insert(pair<string, string>("Content-Length", to_string(lenght)));
		}
	}
	response += version + " " + status_code + " " + reason_phrase + "\r\n";
	map<string, string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	response += "\r\n" + message;
}

Response::~Response() {}

// getters
string const	&Response::getVersion() const { return version; }
string const	&Response::getStatusCode() const { return status_code; }
string const	&Response::getReasonPhrase() const { return reason_phrase; }
string const	&Response::getResponse() const { return response; }
int const		&Response::getResponsePos() const { return response_pos; }

// setters
void	Response::setResponsePos(int new_pos) { response_pos = new_pos; }

ostream& operator<<(ostream & out, const Response& m)
{
	out << "HTTP Response:" << endl;
	// out << "\tMethod: " << m.version << endl;
	// out << "\tPath: " << m.status_code << endl;
	// out << "\tVersion: " << m.reason_phrase << endl;
	// out << "\tHeaders: " << endl;
	// unordered_map<string, string>::const_iterator it = m.headers.begin();
	// while (it != m.headers.end())
	// {
	// 	out << "\t\t" << it->first << ": " << it->second << endl;
	// 	++it;
	// }
	// out << "\tMessage: " << m.message << endl;
	out << "Response: "<< endl << m.response << endl;
	return out;
}