#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

enum Commands {
	UNDEFINED,
	NICK,
	USER,
	JOIN,
	PRVMSG
};

struct Message 
{
	Message() : cmd(UNDEFINED) {};
	int							cmd;
	std::vector<std::string>	parameters;
};

#include "Server.hpp"
#include "Client.hpp"
#include "Reply.hpp"

class MessageHandler
{
	public : // REDUNDANT MSGLIST CONSTRUCTION
		MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector);
		~MessageHandler();

		void	operator()(struct Message msg);

		void	handleMsg();
		void	sendReply(int code);

		//Getters
		std::list<Message> *getMsgList(); // REDUNDANT

	private :
		std::vector<Client *>	_clientVector;
		Client *				_client;
		std::list<Message>		_msgList; // REDUNDANT
		Message					_message;
		void	_userCmd();
		void	_nickCmd();
		void	_joinCmd();
		void	_prvMsgCmd();
};

std::ostream& operator<<(std::ostream& os, MessageHandler& mh);
std::ostream& operator<<(std::ostream& os, const Message& m);

#endif
