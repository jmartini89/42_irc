#include "Client.hpp"

Client::Client() : _registered(false), _allowed(false) { this->_buffer.clear();}

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
Client::getBuffer() { return this->_buffer; }

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

bool
Client::isAllowed() const { return this->_allowed; }

/*
* SETTERS
*/

void
Client::setFdSocket(int fdSocket) { this->_fdSocket = fdSocket; }

void
Client::addBuffer(std::string buffer) { this->_buffer += buffer; }

void
Client::clearBuffer() {
	size_t pos = MessageParser::findLastOf(this->_buffer, CRLF);
	// std::cout << "pos:" << pos << std::endl;
	// std::cout << "buffer size:" << this->_buffer.size() << std::endl;

	if (pos == this->_buffer.size() - CRLF.length())
		return this->_buffer.clear();
	
	// std::cout << "buffer before:" << _buffer << std::endl;

	this->_buffer.replace(0, pos, &this->_buffer[pos + CRLF.length()]);
	this->_buffer.erase(this->_buffer.begin() + pos, this->_buffer.end());

	// std::cout << "buffer after:" << _buffer << std::endl;

}

void
Client::setRegistered(bool state) { this->_registered = state; }

void
Client::setHostname(char * hostname) { this->_hostname = hostname; }

void
Client::setNick(std::string nick) { this->_nick = nick; }

void
Client::setUser(std::string user) { this->_user = user; }

void
Client::setRealName(std::string realName) { this->_realName = realName; }

void
Client::setAllowed(bool state) { this->_allowed = state; }

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
