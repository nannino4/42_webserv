#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

#include "Cgi.hpp"

class Response
{
	private:
		std::string							version;
		std::string							status_code;
		std::string							reason_phrase;
		std::map<std::string, std::string>	headers;
		std::string							body;
		std::string							response;
		int									response_pos;
		Cgi									cgi;
		bool								is_complete;
		int									cgi_data_pos;

	public:
		// default constructor
		Response();
		// copy constructor
		Response(const Response &other);
		// assign operator overload
		Response& operator=(const Response &other);
		// destructor
		~Response();

		// getters
		const std::string						&getVersion() const;
		const std::string						&getStatusCode() const;
		const std::string						&getReasonPhrase() const;
		const std::map<std::string,std::string>	&getHeaders() const;
		const std::string						&getBody() const;
		const std::string						&getResponse() const;
		const int								&getResponsePos() const;
		Cgi const								&getCgi() const;
		bool const								&isComplete() const;
		const int								&getCgiDataPos() const;

		// setters
		void	setVersion(std::string const &new_version);
		void	setStatusCode(std::string const &new_status_code);
		void	setReasonPhrase(std::string const &new_reason_phrase);
		void	addNewHeader(std::pair<std::string,std::string> const &new_header);
		void	setBody(std::string const &nw_body);
		void	setResponse(std::string const &new_response);
		void	setResponsePos(int new_pos);
		void	setIsComplete(bool new_is_complete);
		void	setCgiDataPos(int new_pos);

		// other methods
		void	generateErrorPage();
		void	createResponse();
		void	initCgi(Event *event, Request &request);
		int		runCgi(std::string const &path);

		// operator overloads
		friend std::ostream& operator<<(std::ostream & out, const Response& m);
};
