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
		// aliases
		typedef std::map<std::string, std::string>::const_iterator const_iterator;
	
	private:
		// attributes
		std::string	method;
		std::string	path;
		std::string	version;
		std::map<std::string, std::string>	headers;
		std::string	message;

		// coplien form
		Request();
		Request(const Request &);
		Request& operator=(const Request &);

	public:
		// constructor
		Request(const std::string &raw_request);

		// destructor
		~Request();

		// getters
		const std::string	&getVersion() const;
		const std::string	&getMethod() const;
		const std::string	&getPath() const;
		std::string			getHostname() const; // WARNING: return of not const and reference string

		// operator overloads
		friend std::ostream	&operator<<(std::ostream &out, const Request &m);
};
