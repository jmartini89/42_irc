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

void replaceMulti(std::string & buffer, std::string from, std::string to) {
	for (std::string::size_type it = 0; (it = buffer.find(from, it)) != std::string::npos;)
	{
		buffer.replace(it, from.size(), to);
		it += to.size();
	}
}

void func(int sockfd)
{
	while (true) {
		std::string buffer;
		std::getline(std::cin, buffer);

		buffer.erase(buffer.end());

		replaceMulti(buffer, "CR", "\r");
		replaceMulti(buffer, "LF", "\n");

		send(sockfd, buffer.c_str(), buffer.size(), 0);
	}
}

int main()
{
	int sockfd;
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