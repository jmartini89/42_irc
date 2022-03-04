#include "MessageHandler.hpp"
#include "Server.hpp"
#include "Reply.hpp"

//TODO use client and clientVector from server
MessageHandler::MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector, Server * server)
{
	this->_msgList = msgList;
	this->_client = client;
	this->_clientVector = clientVector;
	this->_server = server;
}

MessageHandler::~MessageHandler(){};

std::list<Message> *MessageHandler::getMsgList() { return &(this->_msgList); }

//operator called by for_each loop
void MessageHandler::operator()(struct Message msg)
{
	this->_message = msg;
	handleMsg();
}

std::ostream& operator<<(std::ostream& os, MessageHandler& mh)
{
	for (std::list<Message>::iterator it = mh.getMsgList()->begin(); it != mh.getMsgList()->end(); it++) 
		os << *it;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Message& m)
{
	os << "Command: " << m.cmd << "\n";
	os << "Parameters: ";
	for (int i = 0; i < m.parameters.size(); i++)
		os << m.parameters[i] << " ";
	os << std::endl;
	return os;
}

void MessageHandler::handleMsg()
{
	if (!(this->_message.cmd == NICK || this->_message.cmd == USER || this->_message.cmd == PASS)
		&& !this->_client->isRegistered())
		return serverReply(ERR_NOTREGISTERED);

	switch(this->_message.cmd)
	{
		case NICK:		_nickCmd(); break;
		case USER:		_userCmd(); break;
		case JOIN:		_joinCmd(); break;
		case PRIVMSG:	_prvMsgCmd(false); break;
		case NOTICE:	_prvMsgCmd(true); break;
		case PING:		_pongCmd(); break;
		case PONG:		break;
		case PASS:		_passCmd(); break;
		default:		serverReply(ERR_UNKNOWNCOMMAND);
	}
}

void MessageHandler::_nickCmd()
{
	if (this->_message.parameters.size() < 2) return serverReply(ERR_NONICKNAMEGIVEN);

	if (this->_findClient(this->_message.parameters[1]))
			return serverReply(ERR_NICKNAMEINUSE);

	this->_client->setNick(this->_message.parameters[1]);

	if (!this->_client->isRegistered() && this->_client->isUser()) {
		if (!this->_client->isAllowed())
			return 	serverReply(ERR_PASSWDMISMATCH); //TODO kick out client
		this->_client->setRegistered(true);
		this->_welcomeReply();
	}
	else if (this->_client->isUser()) {
		// TODO : BROADCAST
	}
}

void MessageHandler::_userCmd()
{
	if (this->_message.parameters.size() < 5) return serverReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isUser()) return serverReply(ERR_ALREADYREGISTRED); //FIXME(?) check isRegistered() and not isUser

	this->_client->setUser(this->_message.parameters[1]);

	std::string realName;
	if (this->_message.parameters[4][0] == ':') this->_message.parameters[4].erase(0, 1);
	for (int i = 4; i < this->_message.parameters.size(); i++)
		realName += this->_message.parameters[i];
	this->_client->setRealName(realName);

	if (!this->_client->getNick().empty()) {
		if (!this->_client->isAllowed())
			return 	serverReply(ERR_PASSWDMISMATCH); //TODO kick out client
		this->_client->setRegistered(true);
		return this->_welcomeReply();
	}
}

void	MessageHandler::_passCmd()
{
	if (this->_message.parameters.size() == 1)
		return serverReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isRegistered())
		return serverReply(ERR_ALREADYREGISTRED);
	if (this->_server->checkPwd(this->_message.parameters[1]))
		this->_client->setAllowed(true);
	else
		this->_client->setAllowed(false); //needed in case more pwd commands are sent (only the last one must be used)
}

void MessageHandler::_joinCmd()
{}

static std::string paramAsStr(std::vector<std::string>::iterator iter,
								std::vector<std::string>::iterator end)
{
	std::string str;
	for (; iter != end; ++iter)
		str += *iter + " ";
	return str;
}

void MessageHandler::_prvMsgCmd(bool isNotice)
{
	if (this->_message.parameters.size() == 1) {
		if (isNotice) return;
		return (serverReply(ERR_NORECIPIENT));
	}
	if (this->_message.parameters.size() == 2) {
		if (isNotice) return;
		return (serverReply(ERR_NOTEXTTOSEND));
	}
	
	std::string target = this->_message.parameters[1];
	if (target.c_str()[0] == '#')
		;  //TODO handle # for channels

	std::string header = ":" + this->_client->getNick() 
						+ "!" + this->_client->getUser() 
						+ "@" + this->_client->getHostname() 
						+ " " + this->_message.parameters[0] + " "
						+ target;
	std::string text = " :" + paramAsStr(this->_message.parameters.begin() + 2, this->_message.parameters.end());

	Client *targetClient = this->_findClient(target);
	if (!targetClient) {
		if (isNotice) return;
		return (serverReply(ERR_NOSUCHNICK, target));
	}

	std::string msg = header + text;
	sendMsg(targetClient->getFdSocket(), msg);
}

void MessageHandler::_pongCmd() {
	std::string reply = "PONG";
	this->sendMsg(this->_client->getFdSocket(), reply);
}

Client *
MessageHandler::_findClient(std::string nick)
{
	for (int i = 0; i < this->_clientVector.size(); i++)
		if (nick == this->_clientVector[i]->getNick() && this->_clientVector[i]->isConnected())
			return this->_clientVector[i];
	return (NULL);
}

void MessageHandler::_welcomeReply() {
	serverReply(RPL_WELCOME);
	serverReply(RPL_YOURHOST);
	serverReply(RPL_CREATED);
	serverReply(RPL_MYINFO);
}

void
MessageHandler::serverReply(int code, std::string target)
{
	std::string reply = ":" + IRC_NAME + " ";

	std::ostringstream code_s;
	code_s << std::setw(3) << std::setfill('0') << std::to_string(code);
	reply += code_s.str();
	reply += " " + this->_client->getNick() + " ";

	reply += Reply::getReply(code);
	MessageParser::replace(reply, "<nick>", this->_client->getNick());
	MessageParser::replace(reply, "<user>", this->_client->getUser());
	MessageParser::replace(reply, "<host>", this->_client->getHostname());
	MessageParser::replace(reply, "<servername>", IRC_NAME);
	MessageParser::replace(reply, "<command>", this->_message.parameters[0]);
	MessageParser::replace(reply, "<date>", this->_server->getCreationDate());
	MessageParser::replace(reply, "<version>", VERSION);
	MessageParser::replace(reply, "<available user modes>", USER_MODES);
	MessageParser::replace(reply, "<available channel modes>", CHANNEL_MODES);
	MessageParser::replace(reply, "<nickname>", target);


	// Client *targetClient = _findClient(target);
	// if (targetClient)
	// {
	// 	// ..
	// }

	this->sendMsg(this->_client->getFdSocket(), reply);
}

void
MessageHandler::serverReply(int code) {
	serverReply(code, "");
}

void
MessageHandler::sendMsg(int fd, std::string message) {
	message += CRLF;
	send(fd, message.c_str(), message.size(), 0);
}
