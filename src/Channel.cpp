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


/* Getters Setters */

clientMap * Channel::getClientMap()  { return &this->_clientsChannel; }

bool Channel::checkKey(std::string key) const { return this->_key == key; }

bool Channel::isEmpty() const { return this->_clientsChannel.empty(); }

std::string	Channel::getName() const { return this->_name; }

std::string	Channel::getTopic() const { return this->_topic; }

std::string	Channel::getKey() const { return this->_key; }

std::string	Channel::getMode() const { return "+" + this->_modes; }


void Channel::setTopic(std::string topic) { this->_topic = topic; }

/* Modes Channel */

bool Channel::isProtected() const { return !this->_key.empty(); }

bool Channel::isModerated() const { return this->_isMode('m'); }

bool Channel::isTopicLocked() const { return this->_isMode('t'); }

bool Channel::isNoMsgOutside() const { return this->_isMode('n'); }

bool Channel::isLimited() const { return this->_isMode('l'); }

bool Channel::isSecret() const { return this->_isMode('s'); }

bool Channel::setMode(char c, bool toogle)
{
	// check if c is server-implemented, return false otherwise
	// switch case char/function
}


/* Modes User */

bool Channel::hasOperPriv(Client * client) const { return this->_hasPriv(client, 'o'); }

bool Channel::hasVoicePriv(Client * client) const { return this->_hasPriv(client, 'v'); }


/* Private */

bool Channel::_isMode(char c) const
{
	if (this->_modes.find(c) != std::string::npos)
			return true;
	return false;
}

bool Channel::_hasPriv(Client * client, char c) const
{
	clientMap::const_iterator it = this->_clientsChannel.find(client);
	if (it != this->_clientsChannel.end())
	{
		if (it->second.find(c) != std::string::npos)
			return true;
		return false;
	}
	return false;
}

void Channel::_setPriv(Client * client, char c)
{
	clientMap::iterator it = this->_clientsChannel.find(client);
	if (it != this->_clientsChannel.end())
	{
		if (it->second.find(c) != std::string::npos)
			return;
		it->second += c;
	}
	else
		std::cerr << "setOperPriv: client not found" << std::endl;
}
