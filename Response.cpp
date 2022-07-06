// https://www.jmarshall.com/easy/http/#whatis

#include "Response.hpp"

Response::Response(const Request & request, std::map<std::string,Location> loc)
	: message(), response(), locations(loc), path(request.getPath())
{
	version = request.getVersion();
	if (request.getMethod() == "GET")
		get();
	// it ha to be check if the method is allowed
	// {
	// 	std::map<std::string,Location>::iterator it = locations.begin();
	// 	while(it != locations.end())
	// 	{
	// 		if (it->first == path)
	// 		{
	// 			if (it->second.isMethodAllowed("GET"))
	// 			{
	// 				get();
	// 				break;
	// 			}
	// 		}
	// 	++it;
	// 	}
	// }
	response += version + " " + response_status_code + " " + reason_phrase + "\r\n";
	std::unordered_map<std::string, std::string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	response += "\r\n" + message;
}

void Response::get()
{
	std::ifstream file;
	struct stat buf;
	std::stringstream line;

	path.erase(0, 1);
	stat(path.c_str(), &buf);
	if (S_ISDIR(buf.st_mode))
		manageDir();
	else if (S_ISREG(buf.st_mode))
		fileTobody(path);
	else
	{
		fileTobody("error_pages/404.html");
		response_status_code = "404";
		reason_phrase = "File Not Found";
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
		message = line.str();
		response_status_code = "200";
		reason_phrase = "OK";
		// return true;
	}
	else if (file.fail())
	{
		fileTobody("error_pages/403.html");
		response_status_code = "403";
		reason_phrase = "Forbidden";
	}
}

void Response::manageDir()
{
	std::map<std::string,Location>::iterator it = locations.begin();
	while(it != locations.end())
	{
		if (it->first == path)
		{
			if (it->second.isAutoindex())
			{
				generateAutoIndex();
				break;
			}
			if (!it->second.getIndex().empty())
			{
				fileTobody(path + it->second.getIndex());
				break;
			}
		}
		++it;
	}
}

void Response::generateAutoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::stringstream line;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		line << "<html>\n" << "<body>\n" << "<h1>Index of " << path << "</h1>\n";
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
		line << "</table></body>\n</html>";
		message = line.str();
		closedir (dir);
	}
	else
	{
		/* could not open directory */
		perror ("could not open directory");
		exit(EXIT_FAILURE);
	}
}

Response::~Response() {}

std::string Response::getResponse() { return response; }

std::ostream& operator<<(std::ostream & out, const Response& m)
{
	out << "HTTP Response:" << std::endl;
	// out << "\tMethod: " << m.version << endl;
	// out << "\tPath: " << m.response_status_code << endl;
	// out << "\tVersion: " << m.reason_phrase << endl;
	// out << "\tHeaders: " << endl;
	// unordered_map<string, string>::const_iterator it = m.headers.begin();
	// while (it != m.headers.end())
	// {
	// 	out << "\t\t" << it->first << ": " << it->second << endl;
	// 	++it;
	// }
	// out << "\tMessage: " << m.message << endl;
	out << "Response: "<< std::endl << m.response << std::endl;
	return out;
}