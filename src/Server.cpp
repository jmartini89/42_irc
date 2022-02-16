#include "Server.hpp"

Server::Server(const unsigned int port, std::string password)
: _password(password) {

	this->_fdListen = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fdListen == -1) throw std::exception();

	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(port);
	this->_address.sin_addr.s_addr = INADDR_ANY;

	if (fcntl(this->_fdListen, F_SETFL, O_NONBLOCK) == -1)
		throw std::exception();

	if (bind(
		this->_fdListen,
		(struct sockaddr *)&this->_address,
		sizeof(this->_address)))
	{
		throw std::exception();
	}
}

Server::~Server () {}

void
Server::run() {

	listen(this->_fdListen, BACKLOG_IRC);

	this->_kQueue = kqueue();
	this->_monitorEvent.push_back(t_kevent());
	EV_SET(&this->_monitorEvent[0], this->_fdListen,
		EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	if (kevent(this->_kQueue, &this->_monitorEvent[0], 1, NULL, 0, NULL) == -1)
		throw std::exception();

	while(true)
	{
		;
	}
}
