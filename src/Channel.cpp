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

	std::string mode;
	if (op) mode += "o";
	
	this->_clientsChannel[client] += mode;

	std::cerr << client->getNick() << " joined " << this->_name << std::endl;

	return true;
}

void Channel::part(Client * client) { this->_clientsChannel.erase(client); }


/* Channel Properties */

clientMap * Channel::getClientMap()  { return &this->_clientsChannel; }

bool Channel::checkKey(std::string key) const { return this->_key == key; }

bool Channel::isEmpty() const { return this->_clientsChannel.empty(); }

std::string	Channel::getName() const { return this->_name; }

std::string	Channel::getTopic() const { return this->_topic; }

std::string	Channel::getKey() const { return this->_key; }

size_t Channel::getLimit() const { return this->_usersLimit; }

std::string	Channel::getMode() const { return this->_modes; }

void Channel::setTopic(std::string topic) { this->_topic = topic; }


/* Modes */

bool Channel::isProtected() const { return !this->_key.empty(); }

bool Channel::isModerated() const { return this->_isMode('m'); }

bool Channel::isTopicLocked() const { return this->_isMode('t'); }

bool Channel::isNoMsgOutside() const { return this->_isMode('n'); }

bool Channel::isLimited() const { return this->_isMode('l'); }

bool Channel::isSecret() const { return this->_isMode('s'); }

std::string Channel::getParams() const
{
	std::string msg;
	for (int i = 0; i < this->_modes.size(); i++)
	{
		if (this->_modes[i] == 'k') msg += this->getKey() + " ";
		if (this->_modes[i] == 'l') msg += MessageParser::itoaCustom(this->_usersLimit) + " ";
	}
	return msg;
}

bool Channel::hasOperPriv(Client * client) const { return this->_hasPriv(client, 'o'); }

bool Channel::hasVoicePriv(Client * client) const { return this->_hasPriv(client, 'v'); }


bool Channel::setMode(char c, bool toggle, std::string param)
{
	switch (c)
	{
		case 'l': this->_setLimited(toggle, param); break;
		case 'k': this->_setProtected(toggle, param); break;
		case 't': this->_toggleMode(c, toggle); break;
		case 'm': this->_toggleMode(c, toggle); break;
		case 'n': this->_toggleMode(c, toggle); break;
		case 's': this->_toggleMode(c, toggle); break;
		case 'o': this->_setPriv(c, toggle, param); break;
		case 'v': this->_setPriv(c, toggle, param); break;
		default: return false;
	}
	return true;
}


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


/* Modes - Setters */

void Channel::_toggleMode(char c, bool toggle)
{
	std::string character(1, c);
	if (toggle)
	{
		if (!this->_isMode(c))
			this->_modes += c;
	}
	else
		MessageParser::replace(this->_modes, character, "");
}

void Channel::_setProtected(bool toggle, std::string param)
{
	if (toggle)
	{
		this->_key = param;
		if (!this->_isMode('k'))
			this->_modes += "k";
	}
	else
	{
		if (this->checkKey(param))
			MessageParser::replace(this->_modes, "k", "");
	}
}

void Channel::_setLimited(bool toggle, std::string param)
{
	if (toggle)
		this->_usersLimit = std::atoi(param.c_str());
	this->_toggleMode('l', toggle);
}

void Channel::_setPriv(char c, bool toggle, std::string param)
{
	clientMap::iterator it = this->_clientsChannel.begin();

	// searching client by nick (param)
	while (it != this->_clientsChannel.end())
	{
		if ((*it).first->getNick() == param)
			break;
		it++;
	}
	if (it == this->_clientsChannel.end())
	{
		std::cerr << "setPriv: client not found" << std::endl;
		return;
	}

	if (toggle && it->second.find(c) == std::string::npos)
		it->second += c;
	else if (!toggle)
	{
		std::string character(1, c);
		MessageParser::replace(it->second, character, "");
	}
}
