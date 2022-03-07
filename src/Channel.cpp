#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name, std::string key) : _name(name), _key(key) {
	std::cerr << "New channel created: " << name << " " << key << std::endl;
}

Channel::~Channel() {}

bool Channel::operator==(std::string name) { return this->_name == name; }


/* Commands */

bool Channel::join(Client * client, bool op, std::string key) {

	if (this->isProtected() && this->_key != key) return false; // TODO : isProtected check probably not needed

	this->_clientsChannel[client] = op;

	std::cerr << client->getNick() << " joined " << this->_name << std::endl;

	return true;
}

void Channel::part(Client * client) {}


/* Getters */

clientMap * Channel::getClientMap()  { return &this->_clientsChannel; }

bool Channel::isProtected() const { return !this->_key.empty(); }

bool Channel::checkKey(std::string key) const { return this->_key == key; }

bool Channel::isEmpty() const { return this->_clientsChannel.empty(); }
