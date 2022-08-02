#include "utils.hpp"
#include <map>

int	getTimeDifference(struct timespec time_since_last_check)
{
	struct timespec current_time;
	#ifdef __MACH__
		clock_gettime(_CLOCK_REALTIME, &current_time);
	#elif defined(__linux__)
		clock_gettime(CLOCK_BOOTTIME, &current_time);
	#endif

	int then = time_since_last_check.tv_sec;
	int now = current_time.tv_sec;

	return (now - then);
}

std::string date()
{
	std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %X %Z", std::localtime(&t));
	return std::string(mbstr);
}

std::string last_modified(std::string file)
{
	struct stat f;
	char mbstr[100];

	stat(file.c_str(), &f);
	strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %X %Z", std::localtime(&f.st_mtime));
	return std::string(mbstr);
}

std::string content_type(std::string filename)
{
	std::map<std::string, std::string> map;
	map[".aac"] = "audio/aac";
	map[".abw"] = "application/x-abiword";
	map[".arc"] = "application/octet-stream";
	map[".avi"] = "video/x-msvideo";
	map[".azw"] = "application/vnd.amazon.ebook";
	map[".bin"] = "application/octet-stream";
	map[".bz"] = "application/x-bzip";
	map[".bz2"] = "application/x-bzip2";
	map[".csh"] = "application/x-csh";
	map[".css"] = "text/css";
	map[".csv"] = "text/csv";
	map[".doc"] = "application/msword";
	map[".epub"] = "application/epub+zip";
	map[".gif"] = "image/gif";
	map[".htm"] = "text/html";
	map[".html"] = "text/html";
	map[".ico"] = "image/x-icon";
	map[".ics"] = "text/calendar";
	map[".jar"] = "application/java-archive";
	map[".jpeg"] = "image/jpeg";
	map[".jpg"] = "image/jpeg";
	map[".js"] = "application/js";
	map[".json"] = "application/json";
	map[".mid"] = "audio/midi";
	map[".midi"] = "audio/midi";
	map[".mpeg"] = "video/mpeg";
	map[".mpkg"] = "application/vnd.apple.installer+xml";
	map[".odp"] = "application/vnd.oasis.opendocument.presentation";
	map[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	map[".odt"] = "application/vnd.oasis.opendocument.text";
	map[".oga"] = "audio/ogg";
	map[".ogv"] = "video/ogg";
	map[".ogx"] = "application/ogg";
	map[".pdf"] = "application/pdf";
	map[".ppt"] = "application/vnd.ms-powerpoint";
	map[".rar"] = "application/x-rar-compressed";
	map[".rtf"] = "application/rtf";
	map[".sh"] = "application/x-sh";
	map[".svg"] = "image/svg+xml";
	map[".swf"] = "application/x-shockwave-flash";
	map[".tar"] = "application/x-tar";
	map[".tif"] = "image/tiff";
	map[".tiff"] = "image/tiff";
	map[".ttf"] = "application/x-font-ttf";
	map[".vsd"] = "application/vnd.visio";
	map[".wav"] = "audio/x-wav";
	map[".weba"] = "audio/webm";
	map[".webm"] = "video/webm";
	map[".webp"] = "image/webp";
	map[".woff"] = "application/x-font-woff";
	map[".xhtml"] = "application/xhtml+xml";
	map[".xls"] = "application/vnd.ms-excel";
	map[".xml"] = "application/xml";
	map[".xul"] = "application/vnd.mozilla.xul+xml";
	map[".zip"] = "application/zip";
	map[".3gp"] = "video/3gpp audio/3gpp";
	map[".3g2"] = "video/3gpp2 audio/3gpp2";
	map[".7z"] = "application/x-7z-compressed";

	std::map<std::string, std::string>::iterator it = map.find(filename.substr(filename.find_last_of("."), filename.size()));
	if (it != map.end())
		return it->second;
	return "";
}

std::string	my_getcwd()
{
	char temp[4096];
	return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}
