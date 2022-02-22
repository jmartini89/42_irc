#include "MessageHandler.hpp"

MessageHandler::MessageHandler(std::list<Message>	msgList)
{
	this->_msgList = msgList;
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
			_nickCmd(); break;
		case USER:
			_nickCmd(); break;
		case JOIN:
			_joinCmd(); break;
		case PRVMSG:
			_prvMsgCmd(); break;
		default:
			std::cerr << "Message handler function not found" << std::endl;
	}
}

void	MessageHandler::_userCmd()
{}

void	MessageHandler::_nickCmd()
{}

void	MessageHandler::_joinCmd()
{}

void	MessageHandler::_prvMsgCmd()
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
