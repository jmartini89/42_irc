#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>

enum Commands {
	UNDEFINED,
	NICK,
	USER,
	JOIN,
	PRIVMSG,
	PING,
	PONG
};

struct Message 
{
	Message() : cmd(UNDEFINED) {};
	int							cmd;
	std::vector<std::string>	parameters;
};

#include "Client.hpp"

typedef std::map<std::string, int> enumMap;

class MessageHandler
{
	public :
		MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector);
		~MessageHandler();

		void	operator()(struct Message msg);

		void	handleMsg();
		void	sendReply(int code);

		//Getters
		std::list<Message> *getMsgList(); // REDUNDANT ?

	private :
		std::vector<Client *>	_clientVector;
		Client *				_client;
		std::list<Message>		_msgList; // REDUNDANT ?
		Message					_message;

		bool	cmdValidParameters(int required);
		void	_userCmd();
		void	_nickCmd();
		void	_joinCmd();
		void	_prvMsgCmd();
		void	_pongCmd();
};

static enumMap _initMap() {
	enumMap aMap;
	aMap["UNDEFINED"] = UNDEFINED;
	aMap["NICK"] = NICK;
	aMap["USER"] = USER;
	aMap["JOIN"] = JOIN;
	aMap["PRIVMSG"] = PRIVMSG;
	aMap["PING"] = PING;
	aMap["PONG"] = PONG;
	return aMap;
};

const enumMap cmdMap = _initMap();
std::ostream& operator<<(std::ostream& os, MessageHandler& mh);
std::ostream& operator<<(std::ostream& os, const Message& m);

#endif
