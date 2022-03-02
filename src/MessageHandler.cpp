#include "MessageHandler.hpp"
#include "Server.hpp"
#include "Reply.hpp"

MessageHandler::MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector)
{
	this->_msgList = msgList;
	this->_client = client;
	this->_clientVector = clientVector;
}

MessageHandler::~MessageHandler(){};

std::list<Message> *MessageHandler::getMsgList() { return &(this->_msgList); }

//operator called by for_each loop
void MessageHandler::operator()(struct Message msg)
{
	this->_message = msg;
	handleMsg();
}

void MessageHandler::handleMsg()
{
	switch(this->_message.cmd)
	{
		case NICK:		_nickCmd(); break;
		case USER:		_userCmd(); break;
		case JOIN:		_joinCmd(); break;
		case PRIVMSG:	_prvMsgCmd(); break;
		case PING:		_pongCmd(); break;
		case PONG:		break;
		default:		sendReply(ERR_UNKNOWNCOMMAND);
	}
}

bool MessageHandler::cmdValidParameters(int required) {
	return (this->_message.parameters.size() == required);
}

void MessageHandler::_nickCmd()
{
	if (!this->cmdValidParameters(2)) return sendReply(ERR_NONICKNAMEGIVEN);

	for (int i = 0; i < this->_clientVector.size(); i++)
		if (this->_message.parameters[1] == this->_clientVector[i]->getNick())
			return sendReply(ERR_NICKNAMEINUSE);

	this->_client->setNick(this->_message.parameters[1]);

	if (!this->_client->isLogged() && this->_client->isRegistered()) {
		this->_client->setLogged(true);
		sendReply(RPL_WELCOME);
	}
	else if (this->_client->isRegistered()) {
		// TODO : BROADCAST
	}
}

void MessageHandler::_userCmd()
{
	if (!this->cmdValidParameters(5)) return sendReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isRegistered()) return sendReply(ERR_ALREADYREGISTRED);

	this->_client->setUser(this->_message.parameters[1]);

	std::string realName;
	if (this->_message.parameters[4][0] == ':') this->_message.parameters[4].erase(0, 1);
	for (int i = 4; i < this->_message.parameters.size(); i++)
		realName += this->_message.parameters[i];
	this->_client->setRealName(realName);

	if (!this->_client->getNick().empty()) {
		this->_client->setLogged(true);
		return sendReply(RPL_WELCOME);
	}
}

void MessageHandler::_joinCmd()
{}

void MessageHandler::_prvMsgCmd()
{}

void MessageHandler::_pongCmd() {
	std::string reply = "PONG" + CRLF;
	send(this->_client->getFdSocket(), reply.c_str(), reply.size(), 0);
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

void
MessageHandler::sendReply(int code)
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

	reply += CRLF;
	send(this->_client->getFdSocket(), reply.c_str(), reply.size(), 0);
}
