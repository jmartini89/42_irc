#ifndef Server_HPP
#define Server_HPP

#include <vector>
#include <string>
#include <iostream>

#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG_IRC 8

typedef struct kevent t_kevent;

class Server
{
	public:
		Server(const unsigned int port, std::string password);
		~Server();
		void	run();

	private:
		Server() {};
		std::string		_password;

		int					_fdListen;
		struct sockaddr_in	_address;

		char			_buffer[1024];
		//
		std::vector<t_kevent>	_monitorEvent;
		std::vector<t_kevent>	_triggerEvent;
		int				_kQueue;
};

#endif
