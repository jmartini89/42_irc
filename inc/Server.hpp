#ifndef Server_HPP
#define Server_HPP

#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "MessageHandler.hpp"
#include "MessageParser.hpp"
#include "Reply.hpp"

#define BACKLOG_IRC 8
#define IRC_NAME std::string("42IRC")
#define CRLF std::string("\r\n")

typedef struct kevent t_kevent;

class Server
{
	public:
		Server(const unsigned int port, std::string password);
		~Server();
		void		run();
		Client *	findClient(int eventFd);

	private:
		Server() {};
		std::string		_password;

		int					_fdListen;
		struct sockaddr_in	_address;

		char			_buffer[1024];
		int				_kQueue;

		std::vector<t_kevent>	_monitorEvent;
		std::vector<t_kevent>	_triggerEvent;
		std::vector<Client *>	_client;

		void	_addClient();
		void	_constructErr(std::string errstr);

		void	_eventClientHandler(int eventFd);
};

#endif
