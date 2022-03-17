#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name, std::string key) : _name(name), _key(key)
{
	this->_modes = "otn";
	if (!this->_key.empty()) this->_modes += "k";

	this->_topic = "No topic set";
	std::cerr << "New channel created: " << name << " " << key << std::endl;
}

Channel::~Channel() {
	std::cerr << "Channel " << this->_name  << " deleted" << std::endl;
}

bool Channel::operator==(std::string name) { return this->_name == name; }


/* Commands */

bool Channel::join(Client * client, bool op, std::string key)
{
	if (this->isProtected() && this->_key != key) return false; // TODO : isProtected check probably not needed

	std::string mode = "";
	if (op) mode += "o";
	
	this->_clientsChannel[client] += mode;

	std::cerr << client->getNick() << " joined " << this->_name << std::endl;

	return true;
}

void Channel::part(Client * client) { this->_clientsChannel.erase(client); }


/* Getters */

clientMap * Channel::getClientMap()  { return &this->_clientsChannel; }

bool Channel::checkKey(std::string key) const { return this->_key == key; }

bool Channel::isEmpty() const { return this->_clientsChannel.empty(); }

std::string	Channel::getName() const { return this->_name; }

std::string	Channel::getTopic() const { return this->_topic; }


/* Modes */

bool Channel::_isMode(Client * client, char c) const
{
	if (this->_clientsChannel.find(client) != this->_clientsChannel.end())
	{
		if ((*this->_clientsChannel.find(client)).second.find(c) != std::string::npos)
			return true;
		return false;
	}
	return false;
}

bool Channel::isProtected() const { return !this->_key.empty(); }

bool Channel::isOperator(Client * client) const { return this->_isMode(client, 'o'); }


/* Setters */

void Channel::setTopic(std::string topic) { this->_topic = topic; }
