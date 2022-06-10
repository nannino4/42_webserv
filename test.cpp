
#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()
#include <iostream>

#define BUFSIZE 1000000

int main()
{

    struct sockaddr_in addr;
	int ret;
	int optval = true;
	socklen_t len;

    bzero(&addr, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

	//bind 1
    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd1, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	ret = bind(fd1, (struct sockaddr *)&addr, sizeof(addr));
    std::cout << "ret del primo bind = " << ret << std::endl;
	if (ret == -1)
	{
		perror("ERROR");
	}
    bzero(&addr, sizeof(addr));
	ret = getsockname(fd1, (sockaddr *)&addr, &len);					//getsockname
    std::cout << "ret del primo getsockname = " << ret << std::endl;
	std:: cout << "address = " << inet_ntoa(addr.sin_addr) << std::endl;
	std:: cout << "port = " << ntohs(addr.sin_port) << std::endl;

    bzero(&addr, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr("172.31.73.65");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
	
	//bind 2
	std::cout << std::endl;
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd2, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	ret = bind(fd2, (struct sockaddr *)&addr, sizeof(addr));
    std::cout << "ret del secondo bind = " << ret << std::endl;
	if (ret == -1)
	{
		perror("ERROR");
	}
    bzero(&addr, sizeof(addr));
	ret = getsockname(fd2, (sockaddr *)&addr, &len);					//getsockname
    std::cout << "ret del secondo getsockname = " << ret << std::endl;
	std:: cout << "address = " << inet_ntoa(addr.sin_addr) << std::endl;
	std:: cout << "port = " << ntohs(addr.sin_port) << std::endl;
    
	//listen 1
	std::cout << std::endl;
	ret = listen(fd1, 128);
    std::cout << "ret del primo listen = " << ret << std::endl;
	if (ret == -1)
	{
		perror("ERROR");
	}
    bzero(&addr, sizeof(addr));
	ret = getsockname(fd1, (sockaddr *)&addr, &len);					//getsockname
    std::cout << "ret del primo getsockname = " << ret << std::endl;
	std:: cout << "address = " << inet_ntoa(addr.sin_addr) << std::endl;
	std:: cout << "port = " << ntohs(addr.sin_port) << std::endl;
	
	//listen 2
	std::cout << std::endl;
    ret = listen(fd2, 128);
    std::cout << "ret del secondo listen = " << ret << std::endl;
	if (ret == -1)
	{
		perror("ERROR");
	}
    bzero(&addr, sizeof(addr));
	ret = getsockname(fd2, (sockaddr *)&addr, &len);					//getsockname
    std::cout << "ret del secondo getsockname = " << ret << std::endl;
	std:: cout << "address = " << inet_ntoa(addr.sin_addr) << std::endl;
	std:: cout << "port = " << ntohs(addr.sin_port) << std::endl;
}
