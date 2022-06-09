
#include <sys/socket.h>	//socket()
#include <arpa/inet.h>	//inet_addr()
#include <cstring>		//bzero()
#include <unistd.h>		//close()
#include <iostream>

#define BUFSIZE 1000000

int main()
{

    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr("10.11.12.4");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    char buf[BUFSIZE];

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "ret del bind = " << bind(fd, (struct sockaddr *)&addr, sizeof(addr)) << std::endl;
    listen(fd, 128);
    std::cout << "sto ascoltando" << std::endl;
    socklen_t socklen = sizeof(addr);
    int acceptedFd = accept(fd, (sockaddr*)&addr, &socklen);
    std::cout << "address del client = " <<  inet_ntoa(addr.sin_addr) << std::endl;
    recv(acceptedFd, buf, BUFSIZE, 0);
    std::cout << "ho ricevuto:\n" << buf << std::endl;
}
