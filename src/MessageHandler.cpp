#include "MessageHandler.hpp"
#include "Server.hpp"
#include "Reply.hpp"

MessageHandler::MessageHandler(Client * client, Server * server) {
	this->_client = client;
	this->_server = server;
}

MessageHandler::~MessageHandler(){};

void MessageHandler::operator()(struct Message msg)
{
	this->_message = msg;
	handleMsg();
}

void MessageHandler::handleMsg()
{
	if (!(this->_message.cmd == NICK 
		|| this->_message.cmd == USER 
		|| this->_message.cmd == PASS 
		|| this->_message.cmd == QUIT)
		&& !this->_client->isRegistered())
		return serverReply(ERR_NOTREGISTERED);

	switch(this->_message.cmd)
	{
		case NICK:		_nickCmd(); break;
		case USER:		_userCmd(); break;
		case JOIN:		_joinCmd(); break;
		case PART:		_partCmd(); break;
		case PRIVMSG:	_privMsgCmd(false); break;
		case NOTICE:	_privMsgCmd(true); break;
		case NAMES:		_namesCmd(); break;
		case LIST:		_listCmd(); break;
		case INVITE:	_inviteCmd(); break;
		case PING:		_pongCmd(); break;
		case PONG:		break;
		case PASS:		_passCmd(); break;
		case WHO:		break; // TODO ?
		case MODE:		break; // TODO
		case QUIT:		_quitCmd(); break;
		case KICK:		_kickCmd(); break;
		default:		serverReply(ERR_UNKNOWNCOMMAND);
	}
}


/* Commands */

void MessageHandler::_passCmd()
{
	if (this->_message.parameters.size() == 1)
		return serverReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isRegistered())
		return serverReply(ERR_ALREADYREGISTRED);

	if (this->_message.parameters[1][0] == ':') this->_message.parameters[1].erase(0, 1);

	if (this->_server->checkPwd(this->_message.parameters[1]))
		this->_client->setAllowed(true);
	else //needed in case more pwd commands are sent (only the last one must be used)
		this->_client->setAllowed(false);
}

void MessageHandler::_nickCmd()
{
	if (this->_message.parameters.size() < 2) return serverReply(ERR_NONICKNAMEGIVEN);

	if (this->_server->findClient(this->_message.parameters[1])
		&& this->_server->findClient(this->_message.parameters[1]) != this->_client)
			return serverReply(ERR_NICKNAMEINUSE);

	std::string oldNick =this->_client->getNick();
	this->_client->setNick(this->_message.parameters[1]);

	if (!this->_client->isRegistered() && this->_client->isUser()) this->_register();
	else if (this->_client->isRegistered()) {
		std::string msg = defHeader + " " + "NICK" + " " + this->_message.parameters[1];
		msg.replace(1, this->_client->getNick().size(), oldNick);
		this->sendMsg(this->_client->getFdSocket(), msg);
		this->_broadcastAllChannels(msg, true);
	}
}

void MessageHandler::_userCmd()
{
	if (this->_message.parameters.size() < 5) return serverReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isUser()) return serverReply(ERR_ALREADYREGISTRED);

	this->_client->setUser(this->_message.parameters[1]);

	std::string realName;
	if (this->_message.parameters[4][0] == ':') this->_message.parameters[4].erase(0, 1);
	for (size_t i = 4; i < this->_message.parameters.size(); i++)
		realName += this->_message.parameters[i];
	this->_client->setRealName(realName);

	if (!this->_client->getNick().empty()) this->_register();
}

void MessageHandler::_joinCmd() {

	if (this->_message.parameters.size() < 2) return this->serverReply(ERR_NEEDMOREPARAMS);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");

	std::vector<std::string> keyVector;
	if (this->_message.parameters.size() > 2)
		keyVector = MessageParser::split(this->_message.parameters[2], ",");
	while (keyVector.size() != nameVector.size())
		keyVector.push_back("");

	for (size_t it = 0; it < nameVector.size(); it++) {
		bool op = false;

		if (nameVector[it].front() == '#') nameVector[it].erase(0, 1);
		Channel * channel = this->_server->findChannel(nameVector[it]);
		if (channel == NULL) {
			channel = new Channel(nameVector[it], keyVector[it]);
			this->_server->addChannel(channel);
			op = true;
		}

		// client already joined
		if (channel->getClientMap()->count(this->_client)) return;

		if (!channel->join(this->_client, op, keyVector[it]))
			return this->serverReply(ERR_BADCHANNELKEY);

		std::string msg = defHeader
			+ " " + this->_message.parameters[0] + " "
			+ "#" + channel->getName();
		this->_broadcastChannel(channel, msg, false);

		this->_serverReplyName(channel); // TO BE TESTED
	}
}

void MessageHandler::_partCmd() {
	if (this->_message.parameters.size() < 2) return this->serverReply(ERR_NEEDMOREPARAMS);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");
	std::string reason;
	if (this->_message.parameters.size() > 2) reason = this->_message.parameters[2];

	for (size_t i = 0; i < nameVector.size(); i++) {
		if (nameVector[i].front() == '#') nameVector[i].erase(0, 1);

		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL) return this->serverReply(ERR_NOSUCHCHANNEL, "", "#" + nameVector[i]);
		if (!channel->getClientMap()->count(this->_client)) return this->serverReply(ERR_NOTONCHANNEL);

		std::string msg = defHeader
			+ " " + this->_message.parameters[0] + " "
			+ "#" + channel->getName();
		if (!reason.empty()) msg += " " + reason;
		this->_broadcastChannel(channel, msg, false);

		channel->part(this->_client);

		// Once all users in a channel have left that channel, the channel must be destroyed.
		if (channel->isEmpty()) this->_server->removeChannel(channel);
	}
}

void MessageHandler::_privMsgCmd(bool isNotice)
{
	if (this->_message.parameters.size() == 1) {
		if (isNotice) return;
		return (serverReply(ERR_NORECIPIENT));
	}
	if (this->_message.parameters.size() == 2) {
		if (isNotice) return;
		return (serverReply(ERR_NOTEXTTOSEND));
	}

	std::string target = this->_message.parameters[1];
	std::string header = defHeader
						+ " " + this->_message.parameters[0] + " "
						+ target;
	std::string text = " " + this->_message.parameters[2];
	std::string msg = header + text;

	// channel message
	if (target.front() == '#') {
		target.erase(0, 1);
		Channel * channel = this->_server->findChannel(target);
		if (channel == NULL) return this->serverReply(ERR_CANNOTSENDTOCHAN);
		if (!channel->getClientMap()->count(this->_client)) return this->serverReply(ERR_CANNOTSENDTOCHAN);
		this->_broadcastChannel(channel, msg, true);
		return;
	}

	// client to client message
	Client *targetClient = this->_server->findClient(target);
	if (!targetClient) {
		if (isNotice) return;
		return (serverReply(ERR_NOSUCHNICK, target));
	}
	sendMsg(targetClient->getFdSocket(), msg);
}

void MessageHandler::_namesCmd() {

	if (this->_message.parameters.size() < 2) return serverReply(ERR_NORECIPIENT);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");

	for (size_t i = 0; i < nameVector.size(); i++) {

		if (nameVector[i].front() == '#') nameVector[i].erase(0, 1);
		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL) serverReply(ERR_NOSUCHCHANNEL, "", nameVector[i]);
		else this->_serverReplyName(channel);

		/*  ?! why is this here ?! */
		// std::string msg = defHeader
		// 	+ " " + this->_message.parameters[0] + " "
		// 	+ "#" + channel->getName();
		// this->_broadcastChannel(channel, msg, false);
	}
}

void MessageHandler::_listCmd() {

	std::vector<std::string> nameVector;

	if (this->_message.parameters.size() == 1) {
		std::vector<Channel *> * channelVector = this->_server->getChannelVector();
		for (std::vector<Channel *>::iterator it = channelVector->begin(); it < channelVector->end(); ++it)
			nameVector.push_back((*it)->getName());
	}
	else {
		nameVector = MessageParser::split(this->_message.parameters[1], ",");
		for (size_t i = 0; i < nameVector.size(); i++)
			if (nameVector[i].front() == '#') nameVector[i].erase(0, 1);
	}

	for (size_t i = 0; i < nameVector.size(); i++) {
		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL) serverReply(ERR_NOSUCHCHANNEL, "", "#" + nameVector[i]);
		else serverReply(RPL_LIST, "", "#" + channel->getName());
	}
	serverReply(RPL_LISTEND);
}

void MessageHandler::_pongCmd() {

	std::string reply = "PONG";
	this->sendMsg(this->_client->getFdSocket(), reply);
}

void MessageHandler::_quitCmd() {

	std::cerr << "Client " << this->_client->getHostname() << " FD " << this->_client->getFdSocket() << " disconnected" << std::endl;

	close(this->_client->getFdSocket());
	this->_client->setFdSocket(-1);

	std::string header = defHeader
						+ " " + this->_message.parameters[0] + " ";
	std::string text;
	if (this->_message.parameters.size() > 2) text =  " " + this->_message.parameters[2];
	std::string msg = header + text;

	this->sendMsg(this->_client->getFdSocket(), msg);
	this->_broadcastAllChannels(msg, true);

	for (std::vector<Channel *>::iterator it = this->_server->getChannelVector()->begin();
		it != this->_server->getChannelVector()->end(); ++it)
		(*it)->getClientMap()->erase(this->_client);
}


void MessageHandler::_inviteCmd() 
{
	if (this->_message.parameters.size() < 3)
		return (serverReply(ERR_NEEDMOREPARAMS));
	
	Client *toInvite = this->_server->findClient(this->_message.parameters[1]);
	if (!toInvite)
		return (serverReply(ERR_NOSUCHNICK));

	Channel *channel = this->_server->findChannel(this->_message.parameters[2]);
	if (!channel)
		return (serverReply(ERR_NOSUCHCHANNEL, "", this->_message.parameters[2]));

	bool imInChannel = channel->getClientMap()->find(this->_client) != channel->getClientMap()->end();
	if (!imInChannel)
		return (serverReply(ERR_NOTONCHANNEL));
	
	bool imOperator = (*channel->getClientMap()->find(this->_client)).second;
	if (!imOperator)
		return (serverReply(ERR_CHANOPRIVSNEEDED));

	bool hesInChannel = channel->getClientMap()->find(toInvite) != channel->getClientMap()->end();
	if (hesInChannel)
		return (serverReply(ERR_USERONCHANNEL));

	std::string msg = defHeader + " INVITE " + this->_message.parameters[1] + " "+ this->_message.parameters[2];
	this->sendMsg(toInvite->getFdSocket(), msg);
}


void MessageHandler::_kickCmd() 
{
	if (this->_message.parameters.size() < 3)
		return serverReply(ERR_NEEDMOREPARAMS);

	std::vector<std::string> channelsName = MessageParser::split(this->_message.parameters[1], ",");
	int channelCount = channelsName.size();

	std::vector<std::string> usersName = MessageParser::split(this->_message.parameters[2], ",");
	int usersCount = usersName.size();

	if (channelCount > 1 && channelCount != usersCount)
		return serverReply(ERR_BADCHANMASK);

	std::string reason;
	if (this->_message.parameters.size() == 4)
		reason = this->_message.parameters[3];

	for (int i = 0; i < channelCount; i++)
	{
		Channel *channel = this->_server->findChannel(channelsName[i]);
		if (!channel)
		{
			serverReply(ERR_NOSUCHCHANNEL, "", channelsName[i]);
			continue;
		}

		bool imInChannel = channel->getClientMap()->find(this->_client) != channel->getClientMap()->end();
		if (!imInChannel)
		{
			serverReply(ERR_NOTONCHANNEL);
			continue;
		}

		bool imOperator = (*channel->getClientMap()->find(this->_client)).second;
		if (!imOperator)
		{
			serverReply(ERR_CHANOPRIVSNEEDED);
			continue;
		}

		for (int j = 0; j < usersName.size(); j++)
		{
			/* This "if" statement is here to handle this rule from RFC:
			 * 		For the message to be syntactically correct, there MUST be
   			 * 		either one channel parameter and multiple user parameter, or as many
   			 * 		channel parameters as there are user parameters */
			if (channelCount > 1)
				j = i;					
			
			Client *client = this->_server->findClient(usersName[j]);
			bool hesInChannel = false;
			if (client)
				hesInChannel = channel->getClientMap()->find(client) != channel->getClientMap()->end();
			if (hesInChannel)
			{
				std::string msg = defHeader + " KICK "+ channelsName[i]+ " " +  usersName[j] ;
				if (!reason.empty())
					msg +=  " :" + reason;
				this->_broadcastChannel(channel, msg, false);
				channel->part(client);
			}
			else
				serverReply(ERR_USERNOTINCHANNEL, usersName[j], channelsName[i]);
		}
		if (channel->getClientMap()->empty())
			this->_server->removeChannel(channel);
	}
}

/* Server Operations */

void MessageHandler::_register() {

	if (!this->_client->isAllowed())
	{
		serverReply(ERR_PASSWDMISMATCH);
		std::cerr << "Client " << this->_client->getHostname() << " FD " << this->_client->getFdSocket() << " disconnected" << std::endl;
		close(this->_client->getFdSocket());
		this->_client->setFdSocket(-1);
		return;
	}
	this->_client->setRegistered(true);
	return this->_welcomeReply();
}

void MessageHandler::_welcomeReply() {

	serverReply(RPL_WELCOME);
	serverReply(RPL_YOURHOST);
	serverReply(RPL_CREATED);
	serverReply(RPL_MYINFO);
}

void MessageHandler::_broadcastChannel(Channel * channel, std::string message, bool excludeMe) {
	for (clientMap::iterator it = channel->getClientMap()->begin(); it != channel->getClientMap()->end(); ++it)
		if (!(it->first == this->_client) || !excludeMe)
			this->sendMsg(it->first->getFdSocket(), message);
}

void MessageHandler::_broadcastAllChannels(std::string message, bool excludeMe) {

	std::set<Client *> clientSet;

	for (std::vector<Channel *>::iterator channel = this->_server->getChannelVector()->begin();
		channel != this->_server->getChannelVector()->end(); channel++)
	{
		if ((*channel)->getClientMap()->count(this->_client))
		{
			for (clientMap::iterator it = (*channel)->getClientMap()->begin(); it != (*channel)->getClientMap()->end(); ++it)
			{
				if (!(it->first == this->_client) || !excludeMe)
				{
					std::pair<std::set<Client *>::iterator,bool> ret;
					ret = clientSet.insert((*it).first);
					if (ret.second == true) this->sendMsg(it->first->getFdSocket(), message);
				}
			}
		}
	}
}

void MessageHandler::_serverReplyName(Channel * channel) {

	for (clientMap::iterator it = channel->getClientMap()->begin(); it != channel->getClientMap()->end(); ++it) {
		std::string nick = it->first->getNick();
		if (it->second ) nick = "@" + nick;
		serverReply(RPL_NAMREPLY, nick, "#" + channel->getName());
	}
	serverReply(RPL_ENDOFNAMES);
}

void MessageHandler::serverReply(int code, std::string target, std::string channel) {

	std::string findChannel;
	if (!channel.empty())
		findChannel = channel.c_str() + 1;
	Channel * channelObj = this->_server->findChannel(findChannel);

	std::string reply = ":" + IRC_NAME + " ";

	std::ostringstream code_s;
	code_s << std::setw(3) << std::setfill('0') << std::to_string(code);
	reply += code_s.str();
	reply += " " + this->_client->getNick() + " ";

	reply += Reply::getReply(code);
	MessageParser::replace(reply, "<nick>", this->_client->getNick());
	MessageParser::replace(reply, "<user>", this->_client->getUser());
	MessageParser::replace(reply, "<host>", this->_client->getHostname());
	MessageParser::replace(reply, "<servername>", IRC_NAME);
	MessageParser::replace(reply, "<command>", this->_message.parameters[0]);
	MessageParser::replace(reply, "<date>", this->_server->getCreationDate());
	MessageParser::replace(reply, "<version>", VERSION);
	MessageParser::replace(reply, "<available user modes>", USER_MODES);
	MessageParser::replace(reply, "<available channel modes>", CHANNEL_MODES);
	MessageParser::replace(reply, "<nickname>", target);
	MessageParser::replace(reply, "<channel>", channel);
	if (channelObj) {
		std::stringstream itoa;
		itoa << channelObj->getClientMap()->size();
		MessageParser::replace(reply, "<topic>", channelObj->getTopic());
		MessageParser::replace(reply, "<# visible>", itoa.str());
	}

	// Client *targetClient = _findClient(target);
	// if (targetClient)
	// {
	// 	// ..
	// }

	this->sendMsg(this->_client->getFdSocket(), reply);
}

void MessageHandler::serverReply(int code) { serverReply(code, ""); }

void MessageHandler::serverReply(int code, std::string target) { serverReply(code, target, ""); }

void MessageHandler::sendMsg(int fd, std::string message) {

	if (fd == -1) return;
	message += CRLF;
	send(fd, message.c_str(), message.size(), 0);
}
