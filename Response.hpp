// https://www.jmarshall.com/easy/http/#whatis
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
using namespace std;

class Request;

class Response
{
	private:
		string							version;
		string							response_status_code;
		string							reason_phrase;
		unordered_map<string, string>	headers;
		string							message;
		string							response;

		Response();
		Response(const Response &);
		Response& operator=(const Response &);

	public:

		Response(const Request & request);

		~Response();

		string getResponse();
		friend ostream& operator<<(ostream & out, const Response& m);
};

#endif
