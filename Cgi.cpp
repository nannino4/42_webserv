#include "Cgi.hpp"
#include "Request.hpp"
#include "utility.hpp"

using namespace std;
Cgi::Cgi(const Request &request){
    this->_env["REDIRECT_STATUS"] = "200";
    this->cgi_header = request.getHeader();
    this->_env["SERVER_SOFTWARE"] = "Weebserv/1.0";
    this->_env["REQUEST_METHOD"] = "GET"; //request.getMethod();
    this->_env["SERVER_PROTOCOL"] = request.getVersion();
    this->_env["PATH_INFO"] = request.getPath();
    this->_env["PATH_TRANSLATED"] = get_working_path() + request.getPath();
    this->_env["HTTP_ACCEPT_LANGUAGE"] = this->cgi_header["Accept-Language"];
    this->_env["HTTP_HOST"] = this->cgi_header["Host"];
    this->_env["HTTP_ACCEPT_ENCODING"] = this->cgi_header["Accept-Encoding"];
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// PATH_TRANSLATED: path assoluta dei file nel server ex: /var/www/html/
	// PATH_INFO = la path richiesta in url ex: www.mysite.it/dir/index.php -> PATH_INFO= /dir/index.php
	// con la fusione dei due si ottiene la path assoluta del file
	// QUERY_STRING: contiene info se :
	//// submit di un form con GET 
	//// link diretto include info dopo '?'
	//// é URL encoded


}

Cgi::~Cgi(){

}

std::string Cgi::run_cgi(std::string file_name){ //script_name=index.php
	//int fd_pipe[2];
	int fd_safe[2];
	pid_t pid;
	//char buffer[65536];
    std::string tmp;
    char **env = map_to_char();

    FILE	*fIn = tmpfile();
    FILE	*fOut = tmpfile();
    long	fdIn = fileno(fIn);
    long	fdOut = fileno(fOut);


    lseek(fdIn, 0, SEEK_SET);
	//pid = 0;
	fd_safe[0] = dup(STDIN_FILENO);
	fd_safe[1] = dup(STDOUT_FILENO);
	if ((pid = fork()) == -1)
		std::cout << "Error: fork" << std::endl;
	if (!pid){
        char * const * nll = nullptr;
        dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve((char*)file_name.c_str(), nll , env); // chiamare php passare filename e passare variabili decodificate
        cout << "NO EXECVE" << endl;
		exit(0);
	}
	else{

        //sleep(30);
		waitpid(-1, NULL, 0);
        lseek(fdOut, 0, SEEK_SET);
        int ret = 1;
        cout << "ret: " << ret << endl;
        char buffer[65536];
		while(ret > 0) {
            memset(buffer, 0, 65536);
            ret = read(fdOut, buffer, 65536 - 1);
            //std::cout << buffer << std::endl
            tmp += buffer;

        }
        std::cout << tmp << endl;
		dup2(STDOUT_FILENO, fd_safe[1]);
		dup2(STDIN_FILENO, fd_safe[0]);
        fclose(fIn);
        fclose(fOut);
        close(fdIn);
        close(fdOut);
		close(fd_safe[0]);
        close(fd_safe[1]);
	}
    new_body =  tmp.substr(tmp.find_first_of('>') + 1, tmp.size());
	return(new_body);
	// php restituisce su stdout````
	// restituire tramite stdout al server
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