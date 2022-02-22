#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <list>
#include <vector>
#include <iostream>


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
	public :
		MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector);
		~MessageHandler();
		void			handleMsg(struct Message msg);
		void		sendReply(int code);

		void	operator()(struct Message msg);

		//Getters
		std::list<Message> *getMsgList();

	private :
		std::vector<Client *>	_clientVector;
		Client *					_client;
		std::list<Message>			_msgList;
		void	_userCmd(struct Message msg);
		void	_nickCmd(struct Message msg);
		void	_joinCmd(struct Message msg);
		void	_prvMsgCmd(struct Message msg);
};

std::ostream& operator<<(std::ostream& os, MessageHandler& mh);
std::ostream& operator<<(std::ostream& os, const Message& m);

#endif
