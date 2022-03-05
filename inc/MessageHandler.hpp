#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>

#define defHeader ":" + this->_client->getNick() + "!" + this->_client->getUser() + "@" + this->_client->getHostname()

enum Commands {
	UNDEFINED,
	NICK,
	USER,
	JOIN,
	PRIVMSG,
	NOTICE,
	PING,
	PONG,
	PASS
};

struct Message 
{
	Message() : cmd(UNDEFINED) {};
	int							cmd;
	std::vector<std::string>	parameters;
};

class Server;
class Client;

typedef std::map<std::string, int> enumMap;

class MessageHandler
{
	public :
		MessageHandler(Client * client, Server *server);
		~MessageHandler();

		void	operator()(struct Message msg);

		void	handleMsg();

		void	serverReply(int code);
		void	serverReply(int code, std::string target);
		void	sendMsg(int fd, std::string message);

	private :
		std::vector<Client *>	_clientVector;
		Client *				_client;
		Message					_message;
		Server					*_server;

		void	_userCmd();
		void	_nickCmd();
		void	_joinCmd();
		void	_prvMsgCmd(bool isNotice);
		void	_passCmd();
		void	_pongCmd();

		void		_register();
		void		_welcomeReply();
};

static enumMap _initMap() {
	enumMap aMap;
	aMap["UNDEFINED"] =	UNDEFINED;
	aMap["NICK"] =		NICK;
	aMap["USER"] =		USER;
	aMap["JOIN"] =		JOIN;
	aMap["PRIVMSG"] =	PRIVMSG;
	aMap["NOTICE"] =	NOTICE;
	aMap["PING"] =		PING;
	aMap["PONG"] =		PONG;
	aMap["PASS"] =		PASS;
	return aMap;
};

const enumMap cmdMap = _initMap();

#endif
