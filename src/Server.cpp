#include "Server.hpp"

Server::Server(const unsigned int port, std::string password)
: _password(password) {

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
				std::cout << "Client has disconnected" << std::endl;
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

	// Incoming socket connection on the listening socket.
	// Create a new socket for the actual connection to client.
	int clientLen = sizeof(client->getAddressPointer());
	int fdClient = accept(this->_fdListen, (struct sockaddr *)client->getAddressPointer(), (socklen_t *)&clientLen);
	if (fdClient == -1)
	{
		delete client ;
		std::cerr << "Client: accept function failed" << std::endl;
		return ; 
	}

	this->_client.push_back(client);
	client->setFdSocket(fdClient);

	// Put this new socket connection also as a 'filter' event
	// to watch in kqueue, so we can now watch for events on this
	// new socket.
	EV_SET(&this->_monitorEvent[0], client->getFdSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("kevent function failed");

	std::cout << "Client " << fdClient << " has connected" << std::endl;
}

void
Server::_eventClientHandler(int eventFd)
{
	bzero(this->_buffer, sizeof(this->_buffer));
	size_t bytes_read = recv(eventFd, this->_buffer, sizeof(this->_buffer), 0);

	std::list<Message> msgList = MessageParser::parseMsg(this->_buffer);
	MessageHandler msgHandler(msgList, this->findClient(eventFd));
	std::cout << msgHandler;

	//looping through msgList applying the handleMsg function to every Message
	for_each (msgList.begin(), msgList.end(), msgHandler);
}

Client *
Server::findClient(int eventFd)
{
	for (int i = 0; i < this->_client.size(); i++)
		if (eventFd == this->_client[i]->getFdSocket())
			return this->_client[i];
	throw std::runtime_error("findClient: client not found");
}
