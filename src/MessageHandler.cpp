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
		if (msg.parameters[0] == this->_clientVector[i]->getFdSocket())
			return this->_clientVector[i];
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
