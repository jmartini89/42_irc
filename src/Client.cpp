#include "Client.hpp"

Client::Client() : _registered(false) {}

Client::~Client() {}

/*
* GETTERS
*/

int Client::getFdSocket() const { return this->_fdSocket; }

struct sockaddr_in
*Client::getAddressPointer() { return &this->_address; }

std::string
Client::getHostname() const { return this->_hostname; }

std::string
Client::getNick() const { return this->_nick; }

std::string
Client::getUser() const { return this->_user; }

std::string
Client::getRealName() const { return this->_realName; }

bool
Client::isRegistered() const { return this->_registered; }

bool
Client::isUser() const { return !this->_user.empty(); }

bool
Client::isConnected() const { return this->_fdSocket != -1; }

/*
* SETTERS
*/

void
Client::setRegistered(bool state) { this->_registered = state; }

void
Client::setFdSocket(int fdSocket) { this->_fdSocket = fdSocket; }

void
Client:: setHostname(char * hostname) { this->_hostname = hostname; }

void
Client::setNick(std::string nick) { this->_nick = nick; }

void
Client::setUser(std::string user) { this->_user = user; }

void
Client::setRealName(std::string realName) { this->_realName = realName; }

bool
Client::operator== (const Client * rhs) const {
	if (this->_fdSocket == rhs->getFdSocket()) return true;
	return false;
}

bool
Client::operator== (const int eventFd) const {
	if (this->_fdSocket == eventFd) return true;
	return false;
}

bool
Client::operator== (const std::string nick) const {
	if (this->_nick == nick) return true;
	return false;
}
