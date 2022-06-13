// https://www.jmarshall.com/easy/http/#whatis
#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

class Request
{
	private:
		std::string	method;
		std::string	path;
		std::string	version;
		std::map<std::string, std::string>	headers;
		std::string	message;

		Request();
		Request(const Request &);
		Request& operator=(const Request &);

	public:

		typedef std::map<std::string, std::string>::const_iterator HeaderConstIterator;

		Request(const std::string & raw_request);

		~Request();

		const std::string & getVersion() const;
		const std::string & getMethod() const;
		const std::string & getPath() const;
		std::string getRequestHeaderHost() const; // WARNING: return of not const and reference string

		friend std::ostream& operator<<(std::ostream & out, const Request& m);
};

#endif
