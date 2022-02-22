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

void Client::setFdSocket(int fdSocket)
{
	_fdSocket = fdSocket;
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
