#ifndef Server_HPP
#define Server_HPP

#include "Client.hpp"
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include "MessageHandler.hpp"
#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>

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
		std::vector<Client *>	_client;

		int				_kQueue;

		void	_addClient();
		void	_constructErr(std::string errstr);

		//
		void	_eventHandler(int eventFd);
		std::list<Message>	_parseMsg(std::string msg);

		std::vector<std::string>	_split(std::string str, char delimeter) const;

};

#endif
