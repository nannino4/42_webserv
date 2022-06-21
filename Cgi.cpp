#include "Cgi.hpp"

Cgi::Cgi(){
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
	int fd_pipe[2];
	int fd_safe[2];
	pid_t pid;

	char **tmp = new char*[3];
	char buffer[200];
	std::string test = "-f";
	//size_t size;

	// tmp = NULL;
	pid = 0;
	tmp[1] = new char[file_name.size() + 1];
	tmp[1] = strcpy(tmp[1], (char *)file_name.c_str());
	tmp[0] = new char[3];
	tmp[0] = strcpy(tmp[0], (char *)test.c_str());
	tmp[2] = new char[1];
	tmp[2] = 0;
	fd_safe[0] = dup(STDIN_FILENO);
	fd_safe[1] = dup(STDOUT_FILENO);
	if (pipe(fd_pipe) == -1)
		std::cout << "cazzo" << std::endl;
	if ((pid = fork()) == -1)
		std::cout << "cazzo 2" << std::endl;
	if (!pid){
		dup2(fd_pipe[1], STDOUT_FILENO);
		execve("/usr/bin/php", tmp, NULL); // chiamare php passare filename e passare variabili decodificate
		exit(0);
	}
	else{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		while(read(fd_pipe[0], buffer, 200) > 0)
			std::cout << buffer << std::endl;
		dup2(STDOUT_FILENO, fd_safe[1]);
		dup2(STDIN_FILENO, fd_safe[0]);
		close(fd_pipe[0]);
	}
	return("ciao");
	// php restituisce su stdout````
	// restituire tramite stdout al server
}

void Cgi::get_env(void){
	//genera le env da config e request
}


/* char **Cgi::map_to_char(std::map<std::string, std::string> _env){
	//indovina
} */