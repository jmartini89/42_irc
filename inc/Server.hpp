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
#include "Channel.hpp"
#include "MessageHandler.hpp"
#include "MessageParser.hpp"
#include "Reply.hpp"

class Server
{
	public:
		Server(const unsigned int port, std::string password);
		~Server();
		void	run();

		/* Server Properties */
		bool		checkPwd(std::string password);
		std::string	getCreationDate() const;

		/* Clients */
		Client *	findClient(int eventFd);
		Client *	findClient(std::string nick);
		std::vector<Client *> *		getClientVector();

		/* Channels */
		void								addChannel(Channel * channel);
		std::vector<Channel *>::iterator	removeChannel(Channel * channel);
		Channel *							findChannel(std::string name);
		std::vector<Channel *> *			getChannelVector();
		
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
		std::vector<Channel *>	_channelVector;

		void	_setKevents();

		void	_addClient();
		void	_closeClient(int eventFd);
		void	_messageHandler(int eventFd);

		void	_debugMsgList(std::list<Message> msgList, int eventFd);
};

#endif
