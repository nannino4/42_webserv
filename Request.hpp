// https://www.jmarshall.com/easy/http/#whatis
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

class Request
{
	private:
		// attributes
		std::string							method;
		std::string							path;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::string							body;
		bool								is_valid;

	public:
		// default constructor
		Request();
		// constructor
		Request(const std::string &raw_request);

		// copy constructor
		Request(const Request &other);
		// assign operator overload
		Request& operator=(const Request &other);

		// destructor
		~Request();

		// getters
		const std::string						&getVersion() const;
		const std::string						&getMethod() const;
		const std::string						&getPath() const;
		const std::map<std::string,std::string>	&getHeaders() const;
		const std::string						&getBody() const;
		const std::string						&getHostname() const;
		const bool								&isValid() const;

		// operator overloads
		friend std::ostream	&operator<<(std::ostream &out, const Request &m);
};
