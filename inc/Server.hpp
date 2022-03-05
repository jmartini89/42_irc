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
#include <signal.h>

#include "Defines.hpp"
#include "Client.hpp"
#include "MessageHandler.hpp"
#include "MessageParser.hpp"
#include "Reply.hpp"

class Server
{
	public:
		Server(const unsigned int port, std::string password);
		~Server();
		void		run();

		/* runtime */
		Client *	findClient(int eventFd);
		Client *	findClient(std::string nick);
		bool		checkPwd(std::string password);

		/* getters */
		std::vector<Client *>	getClientVector();
		std::string				getCreationDate() const;
		
	private:
		Server() {};
		std::string		_password;
		std::string 	_creationDate;

		int					_fdListen;
		struct sockaddr_in	_address;

		int				_kQueue;
		struct kevent	_monitorEvent;
		struct kevent	_triggerEvent;

		std::vector<Client *>	_clientVector;

		void	_addClient();
		void	_eventClientHandler(int eventFd);

		void	_setSignals();
		void	_debugMsgList(std::list<Message> msgList);
};

#endif
