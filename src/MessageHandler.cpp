#include "MessageHandler.hpp"
#include "Server.hpp"
#include "Reply.hpp"

MessageHandler::MessageHandler(Client * client, Server * server) : _client(client), _server(server) {}

MessageHandler::~MessageHandler() {};

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
		case WHO:		break;
		case WHOIS:		break; // serverReply objs overhaul required
		case WHOWAS:	break;
		case MODE:		_modeCmd(); break;
		case QUIT:		_quitCmd(); break;
		case KICK:		_kickCmd(); break;
		case TOPIC:		_topicCmd(); break;
		case OPER:		_operCmd(); break;
		case KILL:		_killCmd(); break;
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
		this->_broadcastAllMyChannels(msg, true);
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

void MessageHandler::_joinCmd()
{
	if (this->_message.parameters.size() < 2) return this->serverReply(ERR_NEEDMOREPARAMS);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");

	std::vector<std::string> keyVector;
	if (this->_message.parameters.size() > 2)
		keyVector = MessageParser::split(this->_message.parameters[2], ",");
	while (keyVector.size() != nameVector.size())
		keyVector.push_back("");

	for (size_t it = 0; it < nameVector.size(); it++)
	{
		bool op = false;

		Channel * channel = this->_server->findChannel(nameVector[it]);
		if (channel == NULL) {
			channel = new Channel(nameVector[it], keyVector[it]);
			this->_server->addChannel(channel);
			op = true;
		}

		// client already joined
		if (channel->getClientMap()->count(this->_client)) return;

		if (channel->isLimited() && (channel->getClientMap()->size() >= channel->getLimit()))
			return serverReply(ERR_CHANNELISFULL, "", channel->getName());

		if (!channel->join(this->_client, op, keyVector[it]))
			return this->serverReply(ERR_BADCHANNELKEY, "", channel->getName());

		std::string msg = defHeader
			+ " " + this->_message.parameters[0]
			+ " " + channel->getName();
		this->_broadcastChannel(channel, msg, false);

		this->_serverReplyName(channel);
		this->serverReply(RPL_TOPIC, "", channel->getName());
	}
}

void MessageHandler::_partCmd()
{
	if (this->_message.parameters.size() < 2) return this->serverReply(ERR_NEEDMOREPARAMS);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");
	std::string reason;
	if (this->_message.parameters.size() > 2) reason = this->_message.parameters[2];

	for (size_t i = 0; i < nameVector.size(); i++) {

		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL) return this->serverReply(ERR_NOSUCHCHANNEL, "", nameVector[i]);

		std::cout << channel->getName() << std::endl;

		if (!channel->getClientMap()->count(this->_client)) return this->serverReply(ERR_NOTONCHANNEL);

		std::string msg = defHeader
			+ " " + this->_message.parameters[0]
			+ " " + channel->getName();
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
	if (target.front() == '#')
	{
		Channel * channel = this->_server->findChannel(target);
		if (channel == NULL) return this->serverReply(ERR_CANNOTSENDTOCHAN, "", channel->getName());

		if (channel->isNoMsgOutside() && !channel->getClientMap()->count(this->_client))
			return this->serverReply(ERR_CANNOTSENDTOCHAN, "", channel->getName());

		if (channel->isModerated()
			&& !(channel->hasOperPriv(this->_client) || channel->hasVoicePriv(this->_client)))
			return this->serverReply(ERR_CANNOTSENDTOCHAN, "", channel->getName());

		this->_broadcastChannel(channel, msg, true);
		return;
	}

	// client to client message
	Client *targetClient = this->_server->findClient(target);
	if (!targetClient)
	{
		if (isNotice) return;
		return (serverReply(ERR_NOSUCHNICK, target));
	}
	sendMsg(targetClient->getFdSocket(), msg);
}

void MessageHandler::_namesCmd()
{
	if (this->_message.parameters.size() < 2) return serverReply(ERR_NORECIPIENT);

	std::vector<std::string> nameVector = MessageParser::split(this->_message.parameters[1], ",");

	for (size_t i = 0; i < nameVector.size(); i++) {

		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL) serverReply(ERR_NOSUCHCHANNEL, "", nameVector[i]);
		else this->_serverReplyName(channel);
	}
}

void MessageHandler::_listCmd()
{
	std::vector<std::string> nameVector;

	if (this->_message.parameters.size() == 1) {
		std::vector<Channel *> * channelVector = this->_server->getChannelVector();
		for (std::vector<Channel *>::iterator it = channelVector->begin(); it < channelVector->end(); ++it)
			nameVector.push_back((*it)->getName());
	}
	else
		nameVector = MessageParser::split(this->_message.parameters[1], ",");

	for (size_t i = 0; i < nameVector.size(); i++) {
		Channel * channel = this->_server->findChannel(nameVector[i]);
		if (channel == NULL)
			serverReply(ERR_NOSUCHCHANNEL, "", nameVector[i]);
		else if (!channel->isSecret() || channel->getClientMap()->count(this->_client))
			serverReply(RPL_LIST, "", channel->getName());
	}
	serverReply(RPL_LISTEND);
}

void MessageHandler::_pongCmd() { this->sendMsg(this->_client->getFdSocket(), "PONG"); }

void MessageHandler::_modeCmd()
{
	if (this->_message.parameters.size() < 2)
		return serverReply(ERR_NEEDMOREPARAMS);

	Channel * channel = this->_server->findChannel(this->_message.parameters[1]);
	if (!channel)
		return serverReply(ERR_NOSUCHCHANNEL, "", this->_message.parameters[1]);

	if (this->_message.parameters.size() == 2)
		return serverReply(RPL_CHANNELMODEIS, "", channel->getName());

	if (!channel->hasOperPriv(this->_client))
		return serverReply(ERR_CHANOPRIVSNEEDED, "", channel->getName());

	std::string oldMode = channel->getMode();
	bool toggle = true;
	for (size_t i = 0, j = 3; i < this->_message.parameters[2].size(); i++)
	{
		std::string param;

		switch (this->_message.parameters[2][i])
		{
			case '+':	toggle = true; continue;
			case '-':	toggle = false; continue;
			case 'b':	continue;
			default:	break;
		}

		// key, limit, operator, voice
		if (CHANNEL_MODES_PARAM.find(this->_message.parameters[2][i]) != std::string::npos)
		{
			// l requires parameter only for true toggle
			if ((this->_message.parameters[2][i] != 'l' || toggle) && j >= this->_message.parameters.size())
			{
				serverReply(ERR_NEEDMOREPARAMS);
				continue;
			}

			param = this->_message.parameters[j];
			j++;

			// oper & voice dedicated
			if (this->_message.parameters[2][i] == 'o' || this->_message.parameters[2][i] == 'v')
			{
				Client *targetClient = this->_server->findClient(param);
				if (!targetClient)
				{
					serverReply(ERR_NOSUCHNICK, param);
					continue;
				}
				if (!channel->getClientMap()->count(targetClient))
				{
					serverReply(ERR_USERNOTINCHANNEL, param, channel->getName());
					continue;
				}

				// broadcast channel for op/voice change (a bit floody but fine)
				std::string sign = (toggle ? "+" : "-");
				std::string msg = defHeader 
								+ " MODE " + channel->getName()
								+ " " + sign + this->_message.parameters[2][i]
								+ " " + param;
				this->_broadcastChannel(channel, msg, false);
			}
		}

		if (!channel->setMode(this->_message.parameters[2][i], toggle, param))
			serverReply(ERR_UNKNOWNMODE, "", channel->getName());
	}

	std::string newMode = channel->getMode();
	if (oldMode != newMode)
	{
		std::string msgAdd = "+";
		std::string msgRmv = "-";
		std::string msgParam;

		for (size_t i = 0; i < newMode.size(); i++)
		{
			if (oldMode.find(newMode[i]) == std::string::npos)
			{
				msgAdd += newMode[i];
				if (newMode[i] == 'k')
					msgParam += " " + channel->getKey();
				if (newMode[i] == 'l')
					msgParam += " " + MessageParser::itoaCustom(channel->getLimit());
			}
		}
		for (size_t i = 0; i < oldMode.size(); i++)
			if (newMode.find(oldMode[i]) == std::string::npos)
				msgRmv += oldMode[i];

		std::string msg = defHeader 
						+ " MODE " + channel->getName()
						+ " " + msgAdd + msgRmv + msgParam;
		this->_broadcastChannel(channel, msg, false);
	}
}

void MessageHandler::_quitCmd()
{
	std::string header = defHeader
						+ " " + this->_message.parameters[0] + " ";
	std::string text;
	if (this->_message.parameters.size() > 2) text =  " " + this->_message.parameters[2];
	std::string msg = header + text;

	this->sendMsg(this->_client->getFdSocket(), msg);
	this->_broadcastAllMyChannels(msg, true);

	for (std::vector<Channel *>::iterator it = this->_server->getChannelVector()->begin();
		it != this->_server->getChannelVector()->end();)
	{
		(*it)->part(this->_client);
		if ((*it)->getClientMap()->empty())
			it = this->_server->removeChannel(*it);
		else it++;
	}

	std::cerr << "Client " << this->_client->getHostname() << " FD " << this->_client->getFdSocket() << " disconnected" << std::endl;

	close(this->_client->getFdSocket());
	this->_client->setFdSocket(-1);
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
	
	bool imOperator = channel->hasOperPriv(this->_client);
	if (!imOperator)
		return (serverReply(ERR_CHANOPRIVSNEEDED));

	bool hesInChannel = channel->getClientMap()->find(toInvite) != channel->getClientMap()->end();
	if (hesInChannel)
		return (serverReply(ERR_USERONCHANNEL));

	std::string msg = defHeader + " INVITE " + this->_message.parameters[1] + " " + this->_message.parameters[2];
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

		bool imOperator = channel->hasOperPriv(this->_client);
		if (!imOperator)
		{
			serverReply(ERR_CHANOPRIVSNEEDED, "", channel->getName());
			continue;
		}

		for (size_t j = 0; j < usersName.size(); j++)
		{
			/*
			* there MUST be either one channel parameter and multiple user parameter,
			* or as many channel parameters as there are user parameters
			*/
			if (channelCount > 1)
				j = i;

			Client *client = this->_server->findClient(usersName[j]);
			bool hesInChannel = false;
			if (client)
				hesInChannel = channel->getClientMap()->find(client) != channel->getClientMap()->end();
			if (hesInChannel)
			{
				std::string msg = defHeader + " KICK " + channelsName[i]+ " " +  usersName[j] ;
				if (!reason.empty())
					msg +=  " :" + reason;
				this->_broadcastChannel(channel, msg, false);
				channel->part(client);
			}
			else
				serverReply(ERR_USERNOTINCHANNEL, usersName[j], channelsName[i]);
				
			if (channelCount > 1)
				break;
		}
		if (channel->getClientMap()->empty())
			this->_server->removeChannel(channel);
	}
}

void MessageHandler::_topicCmd()
{
	if (this->_message.parameters.size() < 2) return serverReply(ERR_NEEDMOREPARAMS);

	Channel * channel = this->_server->findChannel(this->_message.parameters[1]);
	if (!channel)
		return serverReply(ERR_NOSUCHCHANNEL, "", this->_message.parameters[1]);

	bool imInChannel = (channel->getClientMap()->find(this->_client) != channel->getClientMap()->end());
	if (!imInChannel)
		return serverReply(ERR_NOTONCHANNEL);

	if (this->_message.parameters.size() == 3)
	{
		if (channel->isTopicLocked() && !channel->hasOperPriv(this->_client))
			return serverReply(ERR_CHANOPRIVSNEEDED, "", channel->getName());

		if (this->_message.parameters[2].front() == ':') this->_message.parameters[2].erase(0, 1);
		channel->setTopic(this->_message.parameters[2]);
	}

	std::string msg = defHeader
					+ " TOPIC " + channel->getName()
					+ " :" + channel->getTopic();
	this->_broadcastChannel(channel, msg, false);
}

void MessageHandler::_operCmd()
{
	if (this->_message.parameters.size() < 3)
		return serverReply(ERR_NEEDMOREPARAMS);

	if (this->_message.parameters[1] != OPNAME || this->_message.parameters[2] != OPPASS)
		return serverReply(ERR_PASSWDMISMATCH);

	this->_client->setServOp();
	serverReply(RPL_YOUREOPER);
}

void MessageHandler::_killCmd()
{
	if (!this->_client->isServOp())
		return serverReply(ERR_NOPRIVILEGES);

	if (this->_message.parameters.size() < 2)
		return serverReply(ERR_NEEDMOREPARAMS);

	Client * target = this->_server->findClient(this->_message.parameters[1]);
	if (!target || !target->isRegistered())
		return serverReply(ERR_NOSUCHNICK, this->_message.parameters[1]);

	MessageHandler msgHandler(target, this->_server);
	struct Message msg;
	msg.cmd = QUIT;
	msg.parameters.push_back("QUIT");
	msgHandler(msg);
}


/* Server Operations */

void MessageHandler::_register()
{
	if (!this->_client->isAllowed())
	{
		serverReply(ERR_PASSWDMISMATCH);
		std::cerr << "Client " << this->_client->getHostname() << " FD " << this->_client->getFdSocket() << " disconnected" << std::endl;
		close(this->_client->getFdSocket());
		this->_client->setFdSocket(-1);
		return;
	}
	this->_client->setRegistered(true);
	this->_welcomeReply();
}

void MessageHandler::_welcomeReply()
{
	serverReply(RPL_WELCOME);
	serverReply(RPL_YOURHOST);
	serverReply(RPL_CREATED);
	serverReply(RPL_MYINFO);
}

void MessageHandler::_broadcastChannel(Channel * channel, std::string message, bool excludeMe)
{
	for (clientMap::iterator it = channel->getClientMap()->begin(); it != channel->getClientMap()->end(); ++it)
		if (!(it->first == this->_client) || !excludeMe)
			this->sendMsg(it->first->getFdSocket(), message);
}

void MessageHandler::_broadcastAllMyChannels(std::string message, bool excludeMe)
{
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
					std::pair<std::set<Client *>::iterator,bool> ret = clientSet.insert((*it).first);
					if (ret.second == true)
						this->sendMsg(it->first->getFdSocket(), message);
				}
			}
		}
	}
}

void MessageHandler::_serverReplyName(Channel * channel)
{
	for (clientMap::iterator it = channel->getClientMap()->begin(); it != channel->getClientMap()->end(); ++it)
	{
		std::string nick = it->first->getNick();
		if (channel->hasOperPriv(it->first)) nick = "@" + nick;
		serverReply(RPL_NAMREPLY, nick, channel->getName());
	}
	serverReply(RPL_ENDOFNAMES);
}

void MessageHandler::serverReply(int code, std::string target, std::string channel)
{
	Channel * channelObj = this->_server->findChannel(channel);

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
	MessageParser::replace(reply, "<channel modes with a parameter>", CHANNEL_MODES_PARAM);
	MessageParser::replace(reply, "<nickname>", target);
	MessageParser::replace(reply, "<channel>", channel);
	if (channelObj) {
		MessageParser::replace(reply, "<topic>", channelObj->getTopic());
		MessageParser::replace(reply, "<# visible>", MessageParser::itoaCustom(channelObj->getClientMap()->size()));
		MessageParser::replace(reply, "<mode>", channelObj->getMode());
		MessageParser::replace(reply, "<mode params>", channelObj->getParams());
	}

	this->sendMsg(this->_client->getFdSocket(), reply);
}

void MessageHandler::serverReply(int code) { serverReply(code, ""); }

void MessageHandler::serverReply(int code, std::string target) { serverReply(code, target, ""); }

void MessageHandler::sendMsg(int fd, std::string message)
{
	if (fd == -1) return;
	message += CRLF;
	send(fd, message.c_str(), message.size(), 0);
}
