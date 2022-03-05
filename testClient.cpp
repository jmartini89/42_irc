#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

static bool replace(std::string& str, std::string from, std::string to)
{
	size_t start = str.find(from);
	if(start == -1)
		return false;
	str.replace(start, from.length(), to);
	return true;
};

void func(int sockfd)
{
	while (true) {
		std::string buffer;
		std::getline(std::cin, buffer);

		buffer.erase(buffer.end());

		// works only with one CRLF per line
		// wasted way too much time for multiples inline, fuckit
		// works tho
		replace(buffer, "CR", "\r");
		replace(buffer, "LF", "\n");

		send(sockfd, buffer.c_str(), sizeof(buffer), 0);
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "socket creation failed..." << std::endl;
		exit(0);
	}
	else std::cerr << "Socket successfully created.." << std::endl;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		std::cerr << "connection with the server failed..." << std::endl;
		exit(0);
	} else std::cerr << "connected to the server.." << std::endl;

	func(sockfd);
	close(sockfd);
}