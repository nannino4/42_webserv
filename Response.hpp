// https://www.jmarshall.com/easy/http/#whatis
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

class Request;

class Response
{
	private:
		string							version;
		string							status_code;
		string							reason_phrase;
		map<string, string>				headers;
		string							message;
		string							response;
		int								response_pos;

	public:
		// default constructor
		Response();
		// constructor
		Response(const Request &request);
		// copy constructor
		Response(const Response &other);
		// assign operator overload
		Response& operator=(const Response &other);
		// destructor
		~Response();

		// getters
		const string	&getVersion() const;
		const string	&getStatusCode() const;
		const string	&getReasonPhrase() const;
		const string	&getResponse() const;
		const int		&getResponsePos() const;

		// setters
		void	setResponsePos(int new_pos);

		// operator overloads
		friend ostream& operator<<(ostream & out, const Response& m);
};
