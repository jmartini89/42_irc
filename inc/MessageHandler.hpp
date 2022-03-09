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
	PART,
	PRIVMSG,
	NOTICE,
	PING,
	PONG,
	PASS,
	MODE,
	WHO,
	QUIT
};

struct Message 
{
	Message() : cmd(UNDEFINED) {};
	int							cmd;
	std::vector<std::string>	parameters;
};

class Server;
class Client;
class Channel;

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
		void	serverReply(int code, std::string target, std::string channel);

		void	sendMsg(int fd, std::string message);

	private :
		Client *				_client;
		Message					_message;
		Server *				_server;

		/* Commands */
		void	_userCmd();
		void	_nickCmd();
		void	_joinCmd();
		void	_partCmd();
		void	_privMsgCmd(bool isNotice);
		void	_passCmd();
		void	_pongCmd();
		void	_quitCmd();

		void	_broadcastChannel(Channel * channel, std::string message);
		void	_broadcastChannel(Channel * channel, std::string message, bool excludeMe);

		/* Server operations */
		void		_register();
		void		_welcomeReply();
};

static enumMap _initMap() {
	enumMap aMap;
	aMap["UNDEFINED"] =	UNDEFINED;
	aMap["NICK"] =		NICK;
	aMap["USER"] =		USER;
	aMap["JOIN"] =		JOIN;
	aMap["PART"] =		PART;
	aMap["PRIVMSG"] =	PRIVMSG;
	aMap["NOTICE"] =	NOTICE;
	aMap["PING"] =		PING;
	aMap["PONG"] =		PONG;
	aMap["PASS"] =		PASS;
	aMap["MODE"] =		MODE;
	aMap["WHO"] =		WHO;
	aMap["QUIT"] =		QUIT;
	return aMap;
};

const enumMap cmdMap = _initMap();

#endif
