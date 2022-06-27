#include "Cgi.hpp"

Cgi::Cgi(){
	// PATH_TRANSLATED: path assoluta dei file nel server ex: /var/www/html/
	// PATH_INFO = la path richiesta in url ex: www.mysite.it/dir/index.php -> PATH_INFO= /dir/index.php
	// con la fusione dei due si ottiene la path assoluta del file
	// QUERY_STRING: contiene info se :
	//// submit di un form con GET 
	//// link diretto include info dopo '?'
	//// é URL encoded
	this->new_body = "";

}

Cgi::~Cgi(){
}

const std::string &Cgi::getNew_body() const{return this->new_body;}

std::string Cgi::run_cgi(std::string file_name){ //script_name=index.php
	int fd_pipe[2];
	int fd_safe[2];
	pid_t pid;

	char **tmp = new char*[2];
	char buffer[200];
	std::string test = "-f";
	std::string full_path = get_working_path()+ "/"+ file_name;
	//size_t size;

	// tmp = NULL;
	pid = 0;
	tmp[0] = new char[full_path.size() + 1];
	tmp[0] = strcpy(tmp[0], (char *)full_path.c_str());
	tmp[0] = new char[3];
	tmp[0] = strcpy(tmp[0], (char *)test.c_str());
	tmp[1] = new char[1];
	tmp[1] = 0;
	fd_safe[0] = dup(STDIN_FILENO);
	fd_safe[1] = dup(STDOUT_FILENO);
	if (pipe(fd_pipe) == -1)
		std::cout << "Error: Pipe" << std::endl;
	if ((pid = fork()) == -1)
		std::cout << "Error: fork" << std::endl;
	if (!pid){
		dup2(fd_pipe[1], STDOUT_FILENO);
		execve("/usr/bin/php-cgi", tmp, NULL); // chiamare php passare filename e passare variabili decodificate
		exit(0);
	}
	else{
		waitpid(pid, NULL, 0);
		close(fd_pipe[1]);
		std::string s_tmp;
		new_body = "<html>\r\n";
		while(read(fd_pipe[0], buffer, 200) > 0)
		{
			if (new_body.empty())
				new_body = buffer;
			else
			{
				s_tmp = new_body;
				new_body = s_tmp + buffer;
			}
		}
		s_tmp = new_body;
		new_body = s_tmp + "\n<!html>";
		dup2(STDOUT_FILENO, fd_safe[1]);
		dup2(STDIN_FILENO, fd_safe[0]);
		close(fd_pipe[0]);
	}
	for (int i=0; i < 3; i++)
		delete[] tmp[i];
	delete[] tmp;
	return(new_body);
	// php restituisce su stdout````
	// restituire tramite stdout al server
}

void Cgi::get_env(void){
	//genera le env da config e request
}


/* char **Cgi::map_to_char(std::map<std::string, std::string> _env){
	//indovina
} */