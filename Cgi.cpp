#include "Cgi.hpp"
#include "utils.hpp"


Cgi::Cgi(const Request &request){
    this->_env["REDIRECT_STATUS"] = "200";
    this->cgi_header = request.getHeaders();
    this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
    this->_env["REQUEST_METHOD"] = request.getMethod();
    this->_env["SERVER_PROTOCOL"] = request.getVersion();
    this->_env["PATH_INFO"] = request.getPath();
    this->_env["PATH_TRANSLATED"] = my_getcwd() + "/" + request.getPath();
    this->_env["HTTP_ACCEPT_LANGUAGE"] = this->cgi_header["Accept-Language"];
    this->_env["HTTP_HOST"] = this->cgi_header["Host"];
    this->_env["HTTP_ACCEPT_ENCODING"] = this->cgi_header["Accept-Encoding"];
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["QUERY_STRING"] = request.getQuery();
    if(this->_env["REQUEST_METHOD"] == "POST") {
        this->_env["CONTENT_LENGTH"] = request.getHeaders().find("Content-Length")->second;
        this->_env["CONTENT_TYPE"] = request.getHeaders().find("Content-Type")->second;
    }
    this->_env["SCRIPT_FILENAME"] = this->_env["PATH_INFO"];
	this->_env["UPLOAD_PATH"] = request.getLocation()->getUploadPath();
    this->post_body_data = request.getBody();
}

Cgi::~Cgi(){}

std::string Cgi::run_cgi(std::string const &file_name){ //script_name=index.php
    int			to_cgi[2];
	int			from_cgi[2];
	pid_t		pid;
    std::string	tmp;
    char		**env = map_to_char();

    pipe(to_cgi);
    pipe(from_cgi);

	if ((pid = fork()) == -1)	//error
		write(from_cgi[1], "Status-code: 500\r\n\r\n", sizeof("Status-code: 500\r\n\r\n"));
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
		//write to cgi
        if(this->_env["REQUEST_METHOD"] == "POST" )
            write(to_cgi[1], this->post_body_data.c_str(), this->post_body_data.size());

		close(to_cgi[0]);
		close(to_cgi[1]);
		close(from_cgi[1]);

		// read from cgi
        int ret = 1;
        char buffer[65536];
		while (ret > 0 || waitpid(pid, nullptr, WNOHANG) == 0)
		{
            memset(buffer, 0, 65536);
            ret = read(from_cgi[0], buffer, 65536 - 1);
            tmp += buffer;
        }
		close(from_cgi[0]);

        // std::cout << "ritorno cgi" << std::endl;
        // std::cout << tmp << std::endl;
	}
    new_body =  tmp;//.substr(tmp.find_first_of('>') + 1, tmp.size());
	return(new_body);
}

char **Cgi::map_to_char(){
    char	**env = new char*[this->_env.size() + 1];
    int	j = 0;
    for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
        std::string	element = i->first + "=" + i->second;
        env[j] = new char[element.size() + 1];
        env[j] = strcpy(env[j], (const char*)element.c_str());
        j++;
    }
    env[j] = nullptr;
    return env;
}
