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

#include "location.hpp"

#include "dirent.h"

class Request;

class Response
{
	private:
		std::string							version;
		std::string							response_status_code;
		std::string							reason_phrase;
		std::unordered_map<std::string, std::string>	headers;
		std::string							message;
		std::string							response;
		std::map<std::string,Location>		locations;
		std::string 						path;

		Response();
		Response(const Response &);
		Response& operator=(const Response &);

	public:

		Response(const Request & request, std::map<std::string,Location> loc);

		~Response();
		
		void get();
		void manageDir();
		void generateAutoIndex();
		bool fileTobody(std::string const & index);

		std::string getResponse();
		friend std::ostream& operator<<(std::ostream & out, const Response& m);
};

#endif
