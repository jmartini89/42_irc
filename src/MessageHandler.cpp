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
	if (msg.parameters[0].size() == 0) return; // REPLY ERROR 431

	for (int i = 0; i < this->_clientVector.size(); i++)
		if (msg.parameters[0] == this->_clientVector[i]->getNick())
			return ;//REPLY ERROR ERR_NICKNAMEINUSE
	this->_client->setNick(msg.parameters[0]);
	sendReply(RPL_WELCOME);
}

void MessageHandler::_userCmd(struct Message msg)
{}

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
	if (code < 10)
	{
		reply += "00";
		reply += code + '0'; //transform int code in the corresponding ascii
	}
	else
		reply += std::to_string(code);
	reply += " ";

	reply += Reply::getReply(code);
	MessageParser::replace(reply, "<nick>", this->_client->getNick());

	// char *test = ":42IRC "RPL_WELCOME" antony :Welcommen!\r\n";
	reply += CRLF;
	std::cout << reply << std::endl;
	send(this->_client->getFdSocket(), reply.c_str(), reply.size(), 0);
}

