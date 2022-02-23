#include "Client.hpp"

Client::Client() : _logged(false) {}

Client::~Client() {}

int Client::getFdSocket() const
{
	return this->_fdSocket;
}

struct sockaddr_in *Client::getAddressPointer()
{
	return &this->_address;
}

std::string
Client::getHostname() {
	return this->_hostname;
}

char *
Client::getHostnameAddress() {
	return this->_hostname;
}

std::string	Client::getNick() const
{
	return this->_nick;
}

std::string		Client::getUser() const
{
	return this->_user;

}

std::string		Client::getRealName() const
{
	return this->_realName;
}

bool
Client::isLogged()
{
	return this->_logged;
}

void
Client::setLogged(bool state)
{
	this->_logged = state;
}

bool
Client::isRegistered()
{
	return !this->_user.empty();
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
