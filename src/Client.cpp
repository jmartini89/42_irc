#include "Client.hpp"

Client::Client() : _logged(false) {}

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
Client::isLogged() const { return this->_logged; }

bool
Client::isRegistered() const { return !this->_user.empty(); }

/*
* SETTERS
*/

void
Client::setLogged(bool state) { this->_logged = state; }

void
Client::setFdSocket(int fdSocket) { this->_fdSocket = fdSocket; }

void
Client::setNick(std::string nick) { this->_nick = nick; }

void
Client:: setHostname(char * hostname) { this->_hostname = hostname; }

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
