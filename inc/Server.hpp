#ifndef Server_HPP
#define Server_HPP

#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <fcntl.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Defines.hpp"
#include "Client.hpp"
#include "MessageHandler.hpp"
#include "MessageParser.hpp"
#include "Reply.hpp"

typedef struct kevent t_kevent;

class Server
{
	public:
		Server(const unsigned int port, std::string password);
		~Server();
		void		run();
		Client *	findClient(int eventFd);
		std::string	getCreationDate() const;
		bool		checkPwd(std::string password);
		
	private:
		Server() {};
		std::string		_password;
		std::string 	_creationDate;
		int					_fdListen;
		struct sockaddr_in	_address;

		int				_kQueue;

		std::vector<t_kevent>	_monitorEvent;
		std::vector<t_kevent>	_triggerEvent;
		std::vector<Client *>	_clientVector;

		void	_addClient();
		void	_constructErr(std::string errstr);

		void	_eventClientHandler(int eventFd);
};

#endif
