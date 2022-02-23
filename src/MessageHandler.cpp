#include "MessageHandler.hpp"
#include "Client.hpp"

MessageHandler::MessageHandler(std::list<Message> msgList, Client * client, const std::vector<Client *> clientVector)
{
	this->_msgList = msgList;
	this->_client = client;
	this->_clientVector = clientVector;
}

MessageHandler::~MessageHandler(){};

std::list<Message> *MessageHandler::getMsgList()
{
	return &(this->_msgList);
}

//operator called by for_each loop
void MessageHandler::operator()(struct Message msg)
{
	handleMsg(msg);
}

void MessageHandler::handleMsg(struct Message msg)
{
	switch(msg.cmd)
	{
		case NICK:
			_nickCmd(msg); break;
		case USER:
			_userCmd(msg); break;
		case JOIN:
			_joinCmd(msg); break;
		case PRVMSG:
			_prvMsgCmd(msg); break;
		default:
			std::cerr << "Message handler function not found" << std::endl;
	}
}

void MessageHandler::_nickCmd(struct Message msg)
{
	if (msg.parameters.empty()) return sendReply(ERR_NONICKNAMEGIVEN);

	for (int i = 0; i < this->_clientVector.size(); i++)
		if (msg.parameters[0] == this->_clientVector[i]->getNick())
			return sendReply(ERR_NICKNAMEINUSE);

	this->_client->setNick(msg.parameters[0]);

	if (!this->_client->isLogged() && this->_client->isRegistered())	// WELCOME
	{
		this->_client->setLogged(true);
		sendReply(RPL_WELCOME);
	}
	else if (this->_client->isRegistered()) {}							// CHANGE + BROADCAST
	else {}																// SET
}

void MessageHandler::_userCmd(struct Message msg)
{
	if (msg.parameters.size() < 4) return sendReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isRegistered()) return sendReply(ERR_ALREADYREGISTRED);

	this->_client->setUser(msg.parameters[0]);


	std::string realName;
	if (msg.parameters[3][0] == ':') msg.parameters[3].erase(0, 1);
	for (int i = 3; i < msg.parameters.size(); i++)
		realName += msg.parameters[i];
	this->_client->setRealName(realName);

	if (!this->_client->getNick().empty()) return sendReply(RPL_WELCOME);
}

void MessageHandler::_joinCmd(struct Message msg)
{}

void MessageHandler::_prvMsgCmd(struct Message msg)
{}

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

void		MessageHandler::sendReply(int code)
{
	std::string reply = ":" + IRC_NAME + " ";

	std::ostringstream code_s;
	code_s << std::setw( 3 ) << std::setfill( '0' ) << std::to_string(code);
	reply += code_s.str();
	reply += " " + this->_client->getNick() + " ";

	reply += Reply::getReply(code);
	MessageParser::replace(reply, "<nick>", this->_client->getNick());
	MessageParser::replace(reply, "<user>", this->_client->getUser());
	MessageParser::replace(reply, "<host>", this->_client->getHostname());
	MessageParser::replace(reply, "<servername>", IRC_NAME);

	// char *test = ":42IRC "RPL_WELCOME" antony :Welcommen!\r\n";
	reply += CRLF;
	send(this->_client->getFdSocket(), reply.c_str(), reply.size(), 0);
}
