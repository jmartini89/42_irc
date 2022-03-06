#include "MessageHandler.hpp"
#include "Server.hpp"
#include "Reply.hpp"

//TODO use client and clientVector from server
MessageHandler::MessageHandler(Client * client, Server * server)
{
	this->_client = client;
	this->_server = server;
	this->_clientVector = server->getClientVector();
}

MessageHandler::~MessageHandler(){};


//operator called by for_each loop
void MessageHandler::operator()(struct Message msg)
{
	this->_message = msg;
	handleMsg();
}

void MessageHandler::handleMsg()
{
	if (!(this->_message.cmd == NICK || this->_message.cmd == USER || this->_message.cmd == PASS)
		&& !this->_client->isRegistered())
		return serverReply(ERR_NOTREGISTERED);

	switch(this->_message.cmd)
	{
		case NICK:		_nickCmd(); break;
		case USER:		_userCmd(); break;
		case JOIN:		_joinCmd(); break;
		case PART:		_partCmd(); break;
		case PRIVMSG:	_prvMsgCmd(false); break;
		case NOTICE:	_prvMsgCmd(true); break;
		case PING:		_pongCmd(); break;
		case PONG:		break;
		case PASS:		_passCmd(); break;
		default:		serverReply(ERR_UNKNOWNCOMMAND);
	}
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
		// BROADCAST : CLIENT && client's joined CHANNELS
		std::string msg = defHeader + " " + "NICK" + " " + this->_message.parameters[1];
		msg.replace(1, this->_client->getNick().size(), oldNick);
		this->sendMsg(this->_client->getFdSocket(), msg);
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

void	MessageHandler::_passCmd()
{
	if (this->_message.parameters.size() == 1)
		return serverReply(ERR_NEEDMOREPARAMS);
	if (this->_client->isRegistered())
		return serverReply(ERR_ALREADYREGISTRED);

	if (this->_message.parameters[1][0] == ':') this->_message.parameters[1].erase(0, 1);

	if (this->_server->checkPwd(this->_message.parameters[1]))
		this->_client->setAllowed(true);
	else
		this->_client->setAllowed(false); //needed in case more pwd commands are sent (only the last one must be used)
}

void MessageHandler::_joinCmd() {

	// MESSAGE STUFF
	std::string channelName = "TODO";
	std::string channelKey = "TODO"; // if empty channel sets as not protected
	bool op = false;

	Channel * channel = this->_server->findChannel(channelName);
	if (channel == NULL) {
		channel = new Channel(channelName, channelKey);
		this->_server->addChannel(channel);
		op = true;
	}

	if (channel->getClients().find(this->_client) != channel->getClients().end()) {
		; // TODO : client already joined
	}

	if (!channel->join(this->_client, op, channelKey)) ; // wrong key reply
}

void MessageHandler::_partCmd() {

	// MESSAGE STUFF
	std::string channelName = "TODO";

	Channel * channel = this->_server->findChannel(channelName);
	std::map<Client *, bool> _clientsChannel = channel->getClients();

	// broadcast part to _clientsChannel

	channel->part(this->_client);

	// Once all users in a channel have left that channel, the channel must be destroyed.
	if (channel->isEmpty()) delete channel;
}

static std::string paramAsStr(std::vector<std::string>::iterator iter,
								std::vector<std::string>::iterator end)
{
	std::string str;
	for (; iter != end; ++iter)
		str += *iter + " ";
	return str;
}

void MessageHandler::_prvMsgCmd(bool isNotice)
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
	if (target[0] == '#')
		;  //TODO handle # for channels

	std::string header = defHeader
						+ " " + this->_message.parameters[0] + " "
						+ target;
	std::string text = " :" + paramAsStr(this->_message.parameters.begin() + 2, this->_message.parameters.end());

	Client *targetClient = this->_server->findClient(target);
	if (!targetClient) {
		if (isNotice) return;
		return (serverReply(ERR_NOSUCHNICK, target));
	}

	std::string msg = header + text;
	sendMsg(targetClient->getFdSocket(), msg);
}

void MessageHandler::_pongCmd() {
	std::string reply = "PONG";
	this->sendMsg(this->_client->getFdSocket(), reply);
}

void MessageHandler::_register() {
	if (!this->_client->isAllowed())
		return serverReply(ERR_PASSWDMISMATCH); //TODO kick out client
	this->_client->setRegistered(true);
	return this->_welcomeReply();
}

void MessageHandler::_welcomeReply() {
	serverReply(RPL_WELCOME);
	serverReply(RPL_YOURHOST);
	serverReply(RPL_CREATED);
	serverReply(RPL_MYINFO);
}

void
MessageHandler::serverReply(int code, std::string target)
{
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

	// Client *targetClient = _findClient(target);
	// if (targetClient)
	// {
	// 	// ..
	// }

	this->sendMsg(this->_client->getFdSocket(), reply);
}

void
MessageHandler::serverReply(int code) {
	serverReply(code, "");
}

void
MessageHandler::sendMsg(int fd, std::string message) {
	message += CRLF;
	send(fd, message.c_str(), message.size(), 0);
}
