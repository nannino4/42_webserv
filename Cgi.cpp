#include "Cgi.hpp"
#include "utils.hpp"

//coplien form
Cgi::Cgi(): env(nullptr), pid(-1) {}
Cgi::Cgi(Cgi const &other) { *this = other; }
Cgi::~Cgi()
{
	if (env)
	{
		for (size_t i = 0; env[i]; ++i)
		{
			delete env[i];
		}
		delete env;
	}
}

Cgi &Cgi::operator=(Cgi const &other)
{
	_env = other.get_Env();
	env = other.getEnv();
	headers = other.getHeaders();
	post_data = other.getPostData();
	triggered_event = other.getEvent();
	pid = other.getPid();
	return *this;
}

//getters
int const								&Cgi::getToCgiFd() const { return to_cgi[1]; }
int const								&Cgi::getFromCgiFd() const { return from_cgi[0]; }
std::map<std::string,std::string> const	&Cgi::get_Env() const { return _env; }
char									**Cgi::getEnv() const { return env; }
std::map<std::string,std::string> const	&Cgi::getHeaders() const { return headers; }
std::string const						&Cgi::getPostData() const { return post_data; }
Event									*Cgi::getEvent() const { return triggered_event; }
pid_t const								&Cgi::getPid() const { return pid; }

//other methods
void Cgi::initialize(Event *event, const Request &request)
{
	triggered_event = event;
	//set env
    headers = request.getHeaders();
    _env["REDIRECT_STATUS"] = "200";
    _env["SERVER_SOFTWARE"] = "Webserv/1.0";
    _env["REQUEST_METHOD"] = request.getMethod();
    _env["SERVER_PROTOCOL"] = request.getVersion();
    _env["PATH_INFO"] = request.getPath();
    _env["PATH_TRANSLATED"] = my_getcwd() + "/" + request.getPath();
    _env["HTTP_ACCEPT_LANGUAGE"] = headers["Accept-Language"];
    _env["HTTP_HOST"] = headers["Host"];
    _env["HTTP_ACCEPT_ENCODING"] = headers["Accept-Encoding"];
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["QUERY_STRING"] = request.getQuery();
    if(_env["REQUEST_METHOD"] == "POST") {
        _env["CONTENT_LENGTH"] = request.getHeaders().find("Content-Length")->second;
        _env["CONTENT_TYPE"] = request.getHeaders().find("Content-Type")->second;
    }
    _env["SCRIPT_FILENAME"] = _env["PATH_INFO"];
	_env["UPLOAD_PATH"] = request.getLocation()->getUploadPath();
	map_to_char();
	//set post data
    post_data = request.getBody();
}

int Cgi::run_cgi(std::string const &file_name)
{
    std::string	tmp;

	//set pipes for I/O between webserv/CGI
    if (pipe(to_cgi) == -1 || pipe(from_cgi) == -1)
	{
		//error
		return (1);
	}
	if ((pid = fork()) == -1)
	{
		//error
		return (1);
 	}
	if (!pid)	//child
	{
		close(to_cgi[1]);
		close(from_cgi[0]);
        dup2(to_cgi[0], STDIN_FILENO);
		dup2(from_cgi[1], STDOUT_FILENO);

        char * const * nll = nullptr;
		execve((char*)file_name.c_str(), nll , env);	//chiamata php
        std::cout << "Status-code: 500\r\n\r\n" << std::endl;
		exit(1);
	}
	else		//parent
	{
		close(to_cgi[0]);
		close(from_cgi[1]);
	}
	return (0);
}

void Cgi::map_to_char(){
    env = new char*[_env.size() + 1];
    int	j = 0;
    for (std::map<std::string, std::string>::const_iterator i = _env.begin(); i != _env.end(); i++) {
        std::string	element = i->first + "=" + i->second;
        env[j] = new char[element.size() + 1];
        env[j] = strcpy(env[j], (const char*)element.c_str());
        j++;
    }
    env[j] = nullptr;
}
