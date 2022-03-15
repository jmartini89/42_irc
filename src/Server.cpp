#include "Server.hpp"

Server::Server(const unsigned int port, std::string password)
: _password(password) {

	std::time_t now = std::time(nullptr);
	this->_creationDate = std::asctime(std::localtime(&now));
	this->_creationDate.pop_back();

	this->_fdListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fdListen == -1) throw std::runtime_error("socket function failed");

	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(port);
	this->_address.sin_addr.s_addr = INADDR_ANY;

	int optval = 1;
	if (setsockopt(this->_fdListen, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)))
		throw std::runtime_error("setsockopt function failed");

	if (fcntl(this->_fdListen, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl function failed");

	if (bind(
		this->_fdListen,
		(struct sockaddr *)&this->_address,
		sizeof(this->_address)))
		throw std::runtime_error("bind function failed");
}

Server::~Server() {
	std::cerr << std::endl << "QUIT" << std::endl;
	close(this->_fdListen);
	for (size_t i = 0; i < this->_clientVector.size(); i++)
		delete this->_clientVector[i];
}

void Server::run()
{
	listen(this->_fdListen, BACKLOG_IRC);

	this->_kQueue = kqueue();
	this->_setKevents();

	while(true)
	{
		int newEvents = kevent(this->_kQueue, NULL, 0, &this->_triggerEvent, 1, NULL);
		if (newEvents == -1) throw std::runtime_error("kevent function failed");

		int eventFd = this->_triggerEvent.ident;
		int eventFlags = this->_triggerEvent.flags;

		switch (this->_triggerEvent.filter) {
			case EVFILT_SIGNAL: return;

			case EVFILT_READ:
				if (eventFlags & EV_EOF) this->_closeClient(eventFd);
				else if (eventFd == this->_fdListen) this->_addClient();
				else this->_messageHandler(eventFd);

			default: break;
		}
	}
}


/* Server Properties */

bool Server::checkPwd(std::string password) { return (this->_password == password); }

std::string Server::getCreationDate() const { return this->_creationDate; }


/* Clients */

Client * Server::findClient(int eventFd)
{
	for (size_t i = 0; i < this->_clientVector.size(); i++)
		if (eventFd == this->_clientVector[i]->getFdSocket())
			return this->_clientVector[i];
	throw std::runtime_error("findClient: client not found");
}

Client * Server::findClient(std::string nick)
{
	for (size_t i = 0; i < this->_clientVector.size(); i++)
		if (nick == this->_clientVector[i]->getNick() && this->_clientVector[i]->isConnected())
			return this->_clientVector[i];
	return (NULL);
}

std::vector<Client *> * Server::getClientVector() { return &this->_clientVector; }


/* Channels */

void Server::addChannel(Channel * channel) { this->_channelVector.push_back(channel); }

std::vector<Channel *>::iterator Server::removeChannel(Channel * channel) {
	for (std::vector<Channel *>::iterator it = this->_channelVector.begin(); it < this->_channelVector.end(); it++)
		if ((*it) == channel) {
			delete (*it);
			return this->_channelVector.erase(it);
		}
	std::cerr << "removeChannel failed" << std::endl;
	return this->_channelVector.end();
}

Channel * Server::findChannel(std::string name) {
	for (size_t i = 0; i < this->_channelVector.size(); i++) {
		if (*this->_channelVector[i] == name)
			return this->_channelVector[i];
	}
	return NULL;
}

std::vector<Channel *> * Server::getChannelVector() { return &this->_channelVector; }


/* private */

void Server::_setKevents()
{
	signal(SIGINT, SIG_IGN);
	EV_SET(&this->_monitorEvent, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(this->_kQueue, &this->_monitorEvent, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent function failed");

	signal(SIGQUIT, SIG_IGN);
	EV_SET(&this->_monitorEvent, SIGQUIT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(this->_kQueue, &this->_monitorEvent, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent function failed");

	EV_SET(&this->_monitorEvent, this->_fdListen,
		EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(this->_kQueue, &this->_monitorEvent, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent function failed");
}

void Server::_addClient()
{
	Client *client = new Client();

	int addrLen = sizeof(client->getAddressPointer());
	int fdClient = accept(
		this->_fdListen, (struct sockaddr *)client->getAddressPointer(), (socklen_t *)&addrLen);
	if (fdClient == -1) {
		delete client;
		std::cerr << "addClient: accept function failed" << std::endl;
		return;
	}

	if (fcntl(this->_fdListen, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl function failed");

	client->setFdSocket(fdClient);

	char host[HOSTNAME_LEN];
	if (!getnameinfo((struct sockaddr *)client->getAddressPointer(), addrLen, host, HOSTNAME_LEN, NULL, 0, NI_NAMEREQD))
		client->setHostname(host);
	else
		client->setHostname(inet_ntoa(client->getAddressPointer()->sin_addr));

	EV_SET(&this->_monitorEvent, client->getFdSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->_kQueue, &this->_monitorEvent, 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("kevent function failed");

	this->_clientVector.push_back(client);

	std::cerr << "Client " << client->getHostname() << " FD " << fdClient << " has connected" << std::endl;
}

void Server::_closeClient(int eventFd)
{
	MessageHandler msgHandler(this->findClient(eventFd), this);
	struct Message msg;
	msg.cmd = QUIT;
	msg.parameters.push_back("QUIT");
	msgHandler(msg);
}

void Server::_messageHandler(int eventFd)
{
	Client * client = this->findClient(eventFd);

	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);

	ssize_t bytesRead = recv(eventFd, buffer, BUFFER_SIZE, 0);
	if (bytesRead == -1) return;
	else if (bytesRead == 0) return;

	client->replaceBuffer(client->getBuffer() + static_cast<std::string>(buffer));

	// EOT not found
	if (client->getBuffer().find(CRLF) == std::string::npos) return;

	std::list<Message> msgList = MessageParser::parseMsg(client->getBuffer());

	// EOT with buffer remainder
	if (MessageParser::findLastOf(client->getBuffer(), CRLF) != int(client->getBuffer().length() - CRLF.length())) {
		client->clearBuffer();
		for (size_t i = 0; i < msgList.back().parameters.size(); i++)
		{
			client->addBuffer(msgList.back().parameters[i]);
			if (i != msgList.back().parameters.size() - 1)
				 client->addBuffer(" ");
		}
		msgList.pop_back();
	}
	else client->clearBuffer();

	this->_debugMsgList(msgList, eventFd);

	MessageHandler msgHandler(client, this);
	for_each(msgList.begin(), msgList.end(), msgHandler);
}

void Server::_debugMsgList(std::list<Message> msgList, int eventFd) {
	std::stringstream os;
	os << "FD: " << eventFd << std::endl;
	for (std::list<Message>::iterator m = msgList.begin(); m != msgList.end(); m++) {
		os << "Command: " << (*m).cmd << "\n";
		os << "Parameters: ";
		for (size_t i = 0; i < (*m).parameters.size(); i++)
			os << (*m).parameters[i] << " ";
		std::cout << os.str() << std::endl;
	}
}
