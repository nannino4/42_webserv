#include "Cgi.hpp"
#include "utils.hpp"


Cgi::Cgi(const Request &request){
    this->_env["REDIRECT_STATUS"] = "";
    //this->cgi_header = request.getHeaders();
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
    this->post_body_data = request.getBody();
    //debug
    this->test_size = this->post_body_data.size();
}

Cgi::~Cgi(){}

std::string Cgi::run_cgi(std::string file_name){ //script_name=index.php
	int fd_safe[2];
    int tocgi[2];
	pid_t pid;
    std::string tmp;
    char **env = map_to_char();
	size_t	written_bytes = 0;	//bytes

    pipe(tocgi);
    FILE	*fIn = tmpfile();
    FILE	*fOut = tmpfile();
    long	fdIn = fileno(fIn);
    long	fdOut = fileno(fOut);

    lseek(fdIn, 0, SEEK_SET);
	fd_safe[0] = dup(STDIN_FILENO);
	fd_safe[1] = dup(STDOUT_FILENO);
	if ((pid = fork()) == -1)
		std::cout << "500 internal server error" << std::endl;
	if (!pid){
        dup2(tocgi[0], STDIN_FILENO);
        char * const * nll = nullptr;
        //dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);

		execve((char*)file_name.c_str(), nll , env); // chiamare php passare filename e passare variabili decodificate
        std::cout << "500 internal server error" << std::endl; //TODO handle error
		exit(0);
	}
	else{
        if(this->_env["REQUEST_METHOD"] == "POST" )
            written_bytes = write(tocgi[1], this->post_body_data.c_str(), this->post_body_data.size());

		//debug
		std::cout << "written_bytes into cgi =\t" << written_bytes << std::endl;
		std::cout << "expected value =\t\t" << post_body_data.size() << std::endl;
		waitpid(-1, NULL, 0);
        lseek(fdOut, 0, SEEK_SET);
        int ret = 1;
        char buffer[65536];
		while(ret > 0) {
            memset(buffer, 0, 65536);
            ret = read(fdOut, buffer, 65536 - 1);
            tmp += buffer;
        }
        std::cout << "ritorno cgi" << std::endl;
        std::cout << tmp << std::endl;
		dup2(STDOUT_FILENO, fd_safe[1]);
		dup2(STDIN_FILENO, fd_safe[0]);
        fclose(fIn);
        fclose(fOut);
        close(fdIn);
        close(fdOut);
		close(fd_safe[0]);
        close(fd_safe[1]);
	}
    new_body =  tmp;//.substr(tmp.find_first_of('>') + 1, tmp.size());
	return(new_body);
}

void Cgi::get_env(void){
	//genera le env da config e request
}


void Cgi::setCgiHeader(std::map<std::string, std::string> resp_header){
    this->cgi_header = resp_header;
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
