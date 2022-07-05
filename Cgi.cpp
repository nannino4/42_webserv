#include "Cgi.hpp"
using namespace std;

Cgi::Cgi(){
	// PATH_TRANSLATED: path assoluta dei file nel server ex: /var/www/html/
	// PATH_INFO = la path richiesta in url ex: www.mysite.it/dir/index.php -> PATH_INFO= /dir/index.php
	// con la fusione dei due si ottiene la path assoluta del file
	// QUERY_STRING: contiene info se :
	//// submit di un form con GET 
	//// link diretto include info dopo '?'
	//// é URL encoded
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["REQUEST_METHOD"] = "GET";
	this->_env["PATH_INFO"] = "/pages/index.php";
	this->_env["PATH_TRANSLATED"] = get_working_path() + this->_env["PATH_INFO"];
	this->_env["SERVER_NAME"] = "127.0.0.1";
	this->_env["REMOTEaddr"] = "127.0.0.1";
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "Weebserv/1.0";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
/* 	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[];
	this->_env[]; */
	this->new_body = "";

}

Cgi::~Cgi(){
}

const std::string &Cgi::getNew_body() const{return this->new_body;}

std::string Cgi::run_cgi(std::string file_name){ //script_name=index.php
	int fd_pipe[2];
	int fd_safe[2];
	pid_t pid;

	//char **tmp = new char*[2];
	char buffer[200];
	std::string test = "-f";
	std::string full_path = get_working_path()+ "/"+ file_name;
	//size_t size;

	// tmp = NULL;
	pid = 0;
/* 	tmp[0] = new char[full_path.size() + 1];
	tmp[0] = strcpy(tmp[0], (char *)full_path.c_str());
	tmp[0] = new char[3];
	tmp[0] = strcpy(tmp[0], (char *)test.c_str());
	tmp[1] = new char[1];
	tmp[1] = 0; */
	fd_safe[0] = dup(STDIN_FILENO);
	fd_safe[1] = dup(STDOUT_FILENO);
	if (pipe(fd_pipe) == -1)
		std::cout << "Error: Pipe" << std::endl;
	if ((pid = fork()) == -1)
		std::cout << "Error: fork" << std::endl;
	if (!pid){
		char **t = map_to_char();
		int ii = -14;
		dup2(fd_pipe[1], STDOUT_FILENO);
		//execve("/usr/bin/php-cgi", tmp, NULL); // chiamare php passare filename e passare variabili decodificate
		execve("/Users/dbalducc/.brew/bin/php-cgi", NULL, t);
		while(t[++ii])
			free(t[ii]);
		free(t);
		exit(0);
	}
	else{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		std::string s_tmp;
		//new_body = "<html>\r\n";
		while(read(fd_pipe[0], buffer, 200) > 0)
		{
			cout << "buffer: " << buffer << endl;
			if (new_body.empty())
				new_body = buffer;
			else
			{
				s_tmp = new_body;
				new_body = s_tmp + buffer;
			}
		}
		s_tmp = new_body;
		new_body = s_tmp;
		std::cout << new_body << std::endl;
		dup2(STDOUT_FILENO, fd_safe[1]);
		dup2(STDIN_FILENO, fd_safe[0]);
		close(fd_pipe[0]);
	}
/* 	for (int i=0; i < 3; i++)
		delete[] tmp[i];
	delete[] tmp; */
	return(new_body);
	// php restituisce su stdout````
	// restituire tramite stdout al server
}

void Cgi::get_env(void){
	//genera le env da config e request
}


char **Cgi::map_to_char(){
	std::map<std::string, std::string>::iterator it;
	char **cgi_env;
	std::string tmp;
	int i;

	it = this->_env.begin();
	i = 0;
	cgi_env = (char **)malloc(this->_env.size() + 1);
	for(it = this->_env.begin(); it != this->_env.end(); it++)
	{
		tmp = it->first + "=" + it->second;
		//std::cout << tmp  << i<< std::endl;
		cgi_env[i] = (char*)malloc(tmp.size() + 1);
		cout << (char*)tmp.c_str() << endl;
		cgi_env[i++] = (char*)tmp.c_str();
	}
	return cgi_env;
}