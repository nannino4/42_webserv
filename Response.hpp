// https://www.jmarshall.com/easy/http/#whatis
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include "Request.hpp"
#include <sys/stat.h>
#include "dirent.h"

#include "server.hpp"
#include "Request.hpp"

#include "Cgi.hpp"

class Request;

class Response
{
	private:
		std::string										version;
		std::string										response_status_code;
		std::string										reason_phrase;
		std::unordered_map<std::string, std::string>	headers;
		std::string										body;
		std::string										response;
		Server											srv;
		Request 										request;

		Response();
		Response(const Response &);
		Response& operator=(const Response &);

	public:

		Response(const Request & request, Server &other);

		~Response();
		
		void get();
		void delet();
		void manageDir();
		void generateAutoIndex();
		void generateErrorPage();
		void fileTobody(std::string const & index);
		void checkMethod(std::string method, void (Response::*f)());

		std::string getResponse();
		friend std::ostream& operator<<(std::ostream & out, const Response& m);
};

#endif
