#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

#include "location.hpp"

class Request
{
	private:
		// attributes
		std::string							request;
		int									request_pos;
		std::string							method;
		std::string							path;
		std::string							directory_path;
		std::string							file_path;
		std::string							query;
		std::string							version;
		std::string							cgi_path;
		std::map<std::string, std::string>	headers;
		std::string							body;
		bool								is_valid;
		bool								is_complete;
		bool								are_headers_complete;
		Location const						*location;

	public:
		// default constructor
		Request();
		// copy constructor
		Request(const Request &other);
		// assign operator overload
		Request& operator=(const Request &other);

		// destructor
		~Request();

		// getters
		const std::string						&getRequest() const;
		const int								&getRequestPos() const;
		const std::string						&getMethod() const;
		const std::string						&getPath() const;
		const std::string						&getDirectoryPath() const;
		const std::string						&getFilePath() const;
		const std::string						&getQuery() const;
		const std::string						&getVersion() const;
		const std::string						&getCgiPath() const;
		const std::map<std::string,std::string>	&getHeaders() const;
		const std::string						&getBody() const;
		const std::string						&getHostname() const;
		const bool								&isValid() const;
		const bool								&isComplete() const;
		const bool								&areHeadersComplete() const;
		Location const							*getLocation() const;

		// setters
		void	setRequest(std::string const new_request);
		void	setRequestPos(int const new_request_pos);
		void	setMethod(std::string const new_method);
		void	setPath(std::string const new_path);
		void	setDirectoryPath(std::string const new_path);
		void	setFilePath(std::string const new_path);
		void	setQuery(std::string const new_query);
		void	setVersion(std::string const new_version);
		void	setCgiPath(std::string const new_version);
		void	addHeader(std::pair<std::string,std::string> const new_header);
		void	setBody(std::string const new_body);
		void	setIsValid(bool const new_is_valid);
		void	setIsComplete(bool const new_is_complete);
		void	setAreHeadersComplete(bool const new_are_headers_complete);
		void	setLocation(Location const *new_location);

		// operator overloads
		friend std::ostream	&operator<<(std::ostream &out, const Request &m);
};
