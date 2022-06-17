#include <map>
#include <string>

class Cgi
{
private:
	std::map<std::string, std::string> _env;
	std::string new_body;
public:
	Cgi();
	~Cgi();
	std::string run_cgi(std::string script_name);
	void get_env(void);
};
