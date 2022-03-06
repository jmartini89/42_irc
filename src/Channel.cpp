#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name, std::string key) : _name(name), _key(key) {}

Channel::~Channel() {}

bool Channel::operator==(std::string name) { return this->_name == name; }


/* Commands */

bool Channel::join(Client * client, bool op, std::string key) {

	if (this->_key != key) return false;

	this->_clientsChannel.insert(std::map<Client *, bool>::value_type(client, true));

	return true;
}

void Channel::part(Client * client) {}


/* Getters */

std::map<Client *, bool> Channel::getClients() const { return this->_clientsChannel; }

bool Channel::isProtected() const { return !this->_key.empty(); }

bool Channel::checkKey(std::string key) const { return this->_key == key; }

bool Channel::isEmpty() const { return this->_clientsChannel.empty(); }
