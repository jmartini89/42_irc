#include "Server.hpp"

Server::Server(const unsigned int port, std::string password)
: _password(password) {

	std::time_t now = std::time(nullptr);
    this->_creationDate = std::asctime(std::localtime(&now));
	this->_creationDate.pop_back();

	this->_fdListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fdListen == -1) throw std::runtime_error("socket function failed");;

	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(port);
	this->_address.sin_addr.s_addr = INADDR_ANY;

	int optval = 1;
	if (setsockopt(this->_fdListen, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)))
		this->_constructErr("setsockopt function failed");

	if (fcntl(this->_fdListen, F_SETFL, O_NONBLOCK) == -1)
		this->_constructErr("fcntl function failed");

	if (bind(
		this->_fdListen,
		(struct sockaddr *)&this->_address,
		sizeof(this->_address)))
		this->_constructErr("bind function failed");
}

void
Server::_constructErr(std::string errstr)
{
	close(this->_fdListen);
	throw std::runtime_error(errstr);
}

Server::~Server() 
{
	close(this->_fdListen);
	//TODO delete client
}

void
Server::run()
{
	listen(this->_fdListen, BACKLOG_IRC);

	this->_kQueue = kqueue();
	this->_monitorEvent.push_back(t_kevent());
	this->_triggerEvent.push_back(t_kevent());

	EV_SET(&this->_monitorEvent[0], this->_fdListen,
		EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent function failed");

	while(true)
	{
		int newEvents = kevent(this->_kQueue, NULL, 0, &this->_triggerEvent[0], 1, NULL);
		if (newEvents == -1)
			throw std::runtime_error("kevent function failed");

		for (int i = 0; i < newEvents && i < 1; i++)
		{
			int eventFd = this->_triggerEvent[i].ident;

			if (this->_triggerEvent[i].flags & EV_EOF)
			{
				// TODO : BROADCAST + (DELETE?) + ETC ...
				Client *client = this->findClient(eventFd);
				client->setFdSocket(-1);
				std::cerr << "Client " << client->getHostname() << " FD " << eventFd << " disconnected" << std::endl;
				close(eventFd);
			}

			else if (eventFd == this->_fdListen)
				_addClient();

			else if (this->_triggerEvent[i].filter & EVFILT_READ)
				_eventClientHandler(eventFd);
		}
	}
}

void	Server::_addClient()
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

	client->setFdSocket(fdClient);

	char host[HOSTNAME_LEN];
	if (!getnameinfo((struct sockaddr *)client->getAddressPointer(), addrLen, host, HOSTNAME_LEN, NULL, 0, NI_NAMEREQD))
		client->setHostname(host);
	else
		client->setHostname(inet_ntoa(client->getAddressPointer()->sin_addr));

	EV_SET(&this->_monitorEvent[0], client->getFdSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("kevent function failed");

	this->_clientVector.push_back(client);

	std::cerr << "Client " << client->getHostname() << " FD " << fdClient << " has connected" << std::endl;
}

void
Server::_eventClientHandler(int eventFd)
{
	Client * client = this->findClient(eventFd);
	char buffer[BUFFER_SIZE];

	bzero(buffer, BUFFER_SIZE);
	size_t bytes_read = recv(eventFd, buffer, BUFFER_SIZE, 0);
	std::string bufferStr = client->getBuffer() + static_cast<std::string>(buffer);

	client->addBuffer(buffer);

	std::cout << "entry" << std::endl;

	// TODO : clear buffer only until last CRLF
	// for example : cmd params CLRF cmd params (END OF MESSAGE FOR REASONS) -> 2nd part should be kept
	if (client->getBuffer().find(CRLF) == std::string::npos)
		return;

	//  std::cout << "buffer before before:" << bufferStr.substr(0, MessageParser::findLastOf(bufferStr, CRLF) + 1);

	std::list<Message> msgList = MessageParser::parseMsg(bufferStr.substr(0, MessageParser::findLastOf(bufferStr, CRLF) + CRLF.length()));
	// std::list<Message> msgList = MessageParser::parseMsg(client->getBuffer());

	// if (MessageParser::findLastOf(client->getBuffer(), CRLF) == client->getBuffer().length() - CRLF.length())

	MessageHandler msgHandler(msgList, client, this->_clientVector, this);

	std::cout << msgHandler; // DEBUG

	for_each (msgList.begin(), msgList.end(), msgHandler);

	client->clearBuffer();
}

Client *
Server::findClient(int eventFd)
{
	for (int i = 0; i < this->_clientVector.size(); i++)
		if (eventFd == this->_clientVector[i]->getFdSocket())
			return this->_clientVector[i];
	throw std::runtime_error("findClient: client not found");
}

std::string		Server::getCreationDate() const
{ 
	return this->_creationDate;
}

bool		Server::checkPwd(std::string password)
{
	return (this->_password == password);
}
