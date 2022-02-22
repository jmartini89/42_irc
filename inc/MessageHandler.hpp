#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <list>
#include <vector>
#include <iostream>

#include "Client.hpp"

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

class MessageHandler
{
	public :
		MessageHandler(std::list<Message> msgList, Client * client);
		~MessageHandler();
		void	handleMsg(struct Message msg);
		void	operator()(struct Message msg);

		//Getters
		std::list<Message> *getMsgList();

	private :
		std::list<Message>	_msgList;
		Client *			_client;
		void	_userCmd();
		void	_nickCmd();
		void	_joinCmd();
		void	_prvMsgCmd();
};

std::ostream& operator<<(std::ostream& os, MessageHandler& mh);
std::ostream& operator<<(std::ostream& os, const Message& m);

#endif
