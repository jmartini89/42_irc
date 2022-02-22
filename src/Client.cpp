#include "Client.hpp"

Client::Client(/* args */)
{
}

Client::~Client()
{
}

int Client::getFdSocket() const
{
	return this->_fdSocket;
}

struct sockaddr_in *Client::getAddressPointer()
{
	return &this->_address;
}

std::string	Client::getNick() const
{
	return this->_nick;
}


void Client::setFdSocket(int fdSocket)
{
	this->_fdSocket = fdSocket;
}

void	Client::setNick(std::string nick)
{
	this->_nick = nick;
}

bool
Client::operator== (const Client * rhs) const
{
	if (this->_fdSocket == rhs->getFdSocket()) return true;
	return false;
}

bool
Client::operator== (const int eventFd) const
{
	if (this->_fdSocket == eventFd) return true;
	return false;
}

bool
Client::operator== (const std::string nick) const
{
	if (this->_nick == nick) return true;
	return false;
}
